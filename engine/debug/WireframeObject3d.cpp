#include "WireframeObject3d.h"
#include "WireframeObject3dCommon.h"
#include "engine/camera/Camera.h"
#include "engine/base/DirectXBase.h"
#include "engine/math/func/Math.h"
#include "engine/3d/ModelManager.h"
#include "engine/math/func/Rendering.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/base/SRVManager.h"
#include <cassert>

//デストラクタ
WireframeObject3d::~WireframeObject3d() {
	delete worldTransform_;
}

//初期化
void WireframeObject3d::Initialize(Camera* camera, ModelType modelType) {
#ifndef Release
	(void)camera;
	(void)modelType;
#endif // !Release

#ifdef _DEBUG
	//DirectXの基盤部分を受け取る
	directXBase_ = WireframeObject3dCommon::GetInstance()->GetDirectXBase();

	//ワールドトランスフォームの生成、初期化
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize(directXBase_, TransformMode::k3d);

	//uv座標
	uvTransform_ = { {1.0f,1.0f},0.0f,{0.0f,0.0f} };

	//カメラにデフォルトカメラを代入
	worldTransform_->SetCamera(camera);
	//カメラをセット
	WireframeObject3dCommon::GetInstance()->CreateCameraResource(worldTransform_->GetCamera()->GetTranslate());

	//マテリアルの初期化
	material_.color = { 0.0f,0.0f,0.0f,1.0f };
	material_.enableLighting = false;
	material_.uvMatrix = Matrix4x4::Identity4x4();
	material_.shininess = 10.0f;

	//モデルタイプ
	if (modelType == ModelType::kSphere) {
		model_ = ModelManager::GetInstance()->FindModel("sphere");
	} else if (modelType == ModelType::kCube) {
		model_ = ModelManager::GetInstance()->FindModel("cube");
	}
	//マテリアルの設定
	model_->SetMaterial(material_);
#endif // _DEBUG
}


//更新
void WireframeObject3d::Update() {
#ifdef _DEBUG
	//Object3dの共通部分の更新
	WireframeObject3dCommon::GetInstance()->Update();
	//UVトランスフォーム更新
	model_->UVTransform(uvTransform_);

	//ノードの設定
	worldTransform_->SetNode(model_->GetModelData().rootNode);

	//ワールドトランスフォーム
	worldTransform_->Update();
#endif // _DEBUG
}

//描画
void WireframeObject3d::Draw() {
#ifdef _DEBUG
	//3Dオブジェクトの共通部分
	WireframeObject3dCommon::GetInstance()->DrawSetting();

	//PSOの設定
	auto pso = WireframeObject3dCommon::GetInstance()->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);

	//ワールドトランスフォーム
	worldTransform_->Draw();

	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, WireframeObject3dCommon::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, SRVManager::GetInstance()->GetGPUDescriptorHandle(WireframeObject3dCommon::GetInstance()->GetSrvIndexPoint()));
	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, SRVManager::GetInstance()->GetGPUDescriptorHandle(WireframeObject3dCommon::GetInstance()->GetSrvIndexSpot()));
	//モデルの描画	
	model_->Draw();
#endif // _DEBUG
}

//親子付け
void WireframeObject3d::Compose(const WorldTransform* parent) {
	worldTransform_->Compose(parent);
}

//親子関係を解除
void WireframeObject3d::Decompose() {
	worldTransform_->Decompose();
}

//カメラのセッター
void WireframeObject3d::SetCamera(Camera* camera) {
	worldTransform_->SetCamera(camera);
	WireframeObject3dCommon::GetInstance()->SetCameraForGPU(camera->GetTranslate());
}

//半径のセッター
void WireframeObject3d::SetRadius(float radius) {
	radius_ = radius;
	Vector3 scale = { radius_,radius_,radius_ };
	worldTransform_->SetScale(scale);
}

// スケールのセッター
void WireframeObject3d::SetScale(const Vector3& scale) {
	worldTransform_->SetScale(scale);
}

// 回転のセッター
void WireframeObject3d::SetRotate(const Vector3& rotate) {
	worldTransform_->SetRotate(rotate);
}

// 平行移動のセッター
void WireframeObject3d::SetTranslate(const Vector3& translate) {
	worldTransform_->SetTranslate(translate);
}

//色のセッター
void WireframeObject3d::SetColor(const Vector4& color) {
	if (model_) {
		model_->SetColor(color);
	}
}

//親のセッター
void WireframeObject3d::SetParent(const WorldTransform* parent) {
	worldTransform_->SetParent(parent);
}

//ブレンドモードのセッター
void WireframeObject3d::SetBlendMode(const BlendMode& blendMode) {
	blendMode_ = blendMode;
}

// 半径のゲッター
float WireframeObject3d::GetRadius() const {
	return radius_;
}

// スケールのゲッター
Vector3 WireframeObject3d::GetScale() const {
	return  worldTransform_->GetScale();
}

// 回転のゲッター
Vector3 WireframeObject3d::GetRotate() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_->GetRotate();
}

// 平行移動のゲッター
Vector3 WireframeObject3d::GetTranslate() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_->GetTranslate();
}

//色のゲッター
Vector4 WireframeObject3d::GetColor() const {
	return model_->GetColor();
}

//ワールド行列のゲッター
WorldTransform* WireframeObject3d::GetWorldTransform() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_;
}

//ワールド座標のゲッター
Vector3 WireframeObject3d::GetWorldPos() {
	return worldTransform_->GetWorldPos();
}
