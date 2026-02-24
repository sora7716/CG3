#include "WireframeObject3d.h"
#include "WireframeObject3dCommon.h"
#include "engine/camera/Camera.h"
#include "engine/base/DirectXBase.h"
#include "engine/math/func/Rendering.h"
#include "engine/base/SRVManager.h"
#include "engine/3d/ModelManager.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/worldTransform/WorldTransform.h"
#include "engine/3d/Model.h"
#include "engine/math/func/Math.h"
#include "engine/debug/WireframeObject3dCommon.h"
#include <cassert>

//デストラクタ
WireframeObject3d::~WireframeObject3d() {
	delete worldTransform_;
}

//初期化
void WireframeObject3d::Initialize(WireframeObject3dCommon* wireframeObject3dCommon, Camera* camera, ModelType modelType) {
	//ワイヤーフレームオブジェクトの共通部分を記録
	wireframeObject3dCommon_ = wireframeObject3dCommon;
#ifdef _DEBUG
	//マテリアルの初期化
	material_.color = { 0.0f,0.0f,0.0f,1.0f };
	material_.enableLighting = false;
	material_.uvMatrix = Matrix4x4::Identity4x4();
	material_.shininess = 10.0f;

	//モデルタイプ
	if (modelType == ModelType::kSphere) {
		model_ = wireframeObject3dCommon_->GetModelManager()->FindModel("sphere");
	} else if (modelType == ModelType::kCube) {
		model_ = wireframeObject3dCommon_->GetModelManager()->FindModel("cube");
	}
	//マテリアルの設定
	model_->SetMaterial(material_);
#endif // _DEBUG

	//モデルタイプをリリース時でも使っている風にしている
	(void)modelType;

	//DirectXの基盤部分を受け取る
	directXBase_ = wireframeObject3dCommon_->GetDirectXBase();
	//ワールドトランスフォームの生成、初期化
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize(directXBase_, TransformMode::k3d);

	//uv座標
	uvTransform_ = { {1.0f,1.0f},0.0f,{0.0f,0.0f} };

	//カメラにデフォルトカメラを代入
	worldTransform_->SetCamera(camera);
	//カメラをセット
	wireframeObject3dCommon_->CreateCameraResource(worldTransform_->GetCamera()->GetTranslate());
}

//更新
void WireframeObject3d::Update() {
#ifdef _DEBUG
	//Object3dの共通部分の更新
	wireframeObject3dCommon_->Update();
	//UVトランスフォーム更新
	model_->UVTransform(uvTransform_);
	//ノードの設定
	worldTransform_->SetNode(model_->GetModelData().rootNode);
#endif // _DEBUG

	//ワールドトランスフォーム
	worldTransform_->Update();

	//Sphere
	sphere_.center = WireframeObject3d::GetWorldPos();
	sphere_.radius = radius_;

	//AABB
	aabb_.min = WireframeObject3d::GetWorldPos() * -worldTransform_->GetScale();
	aabb_.max = WireframeObject3d::GetWorldPos() * worldTransform_->GetScale();

	//OBB
	obb_.center = WireframeObject3d::GetWorldPos();
	obb_.rotate = worldTransform_->GetRotate();
	Rendering::MakeOBBRotateMatrix(obb_.orientations, obb_.rotate);
	obb_.size = worldTransform_->GetScale();
}

//描画
void WireframeObject3d::Draw() {
#ifdef _DEBUG
	//3Dオブジェクトの共通部分
	wireframeObject3dCommon_->DrawSetting();

	//PSOの設定
	auto pso = wireframeObject3dCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);

	//ワールドトランスフォーム
	worldTransform_->Draw();

	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, wireframeObject3dCommon_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, wireframeObject3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(wireframeObject3dCommon_->GetSrvIndexPoint()));
	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, wireframeObject3dCommon_->GetSRVManager()->GetGPUDescriptorHandle(wireframeObject3dCommon_->GetSrvIndexSpot()));
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
	wireframeObject3dCommon_->SetCameraForGPU(camera->GetTranslate());
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

//球のゲッター
Sphere WireframeObject3d::GetSphere() const {
	return sphere_;
}

//AABBのゲッター
AABB WireframeObject3d::GetAABB() const {
	return aabb_;
}

//OBBのゲッター
OBB WireframeObject3d::GetOBB() const {
	return obb_;
}
