#include "Object3d.h"
#include "Object3dCommon.h"
#include "engine/camera/Camera.h"
#include "engine/base/DirectXBase.h"
#include "engine/math/func/Math.h"
#include "engine/3d/ModelManager.h"
#include "engine/worldTransform/WorldTransform.h"
#include "engine/math/func/Rendering.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/base/SRVManager.h"
#include <cassert>

//デストラクタ
Object3d::~Object3d() {
	delete worldTransform_;
}

//初期化
void Object3d::Initialize(Camera* camera) {
	//DirectXの基盤部分を受け取る
	directXBase_ = Object3dCommon::GetInstance()->GetDirectXBase();

	//ワールドトランスフォームの生成、初期化
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize(directXBase_, TransformMode::k3d);

	//uv座標
	uvTransform_ = { {1.0f,1.0f},0.0f,{0.0f,0.0f} };

	//カメラにデフォルトカメラを代入
	worldTransform_->SetCamera(camera);
	//カメラをセット
	Object3dCommon::GetInstance()->CreateCameraResource(worldTransform_->GetCamera()->GetTranslate());

	//マテリアルの初期化
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.uvMatrix = Matrix4x4::Identity4x4();
	material_.shininess = 10.0f;
}


//更新
void Object3d::Update() {
	//Object3dの共通部分の更新
	Object3dCommon::GetInstance()->Update();

	//ワールドトランスフォーム
	worldTransform_->Update();
	if (model_) {
		model_->UVTransform(uvTransform_);
	}
}

//描画
void Object3d::Draw() {
	//3Dオブジェクトの共通部分
	Object3dCommon::GetInstance()->DrawSetting();

	//PSOの設定
	auto pso = Object3dCommon::GetInstance()->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);

	//ワールドトランスフォーム
	worldTransform_->Draw();

	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, Object3dCommon::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, SRVManager::GetInstance()->GetGPUDescriptorHandle(Object3dCommon::GetInstance()->GetSrvIndexPoint()));
	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, SRVManager::GetInstance()->GetGPUDescriptorHandle(Object3dCommon::GetInstance()->GetSrvIndexSpot()));

	//3Dモデルが割り当てられていれば描画
	if (model_) {
		model_->Draw();
	}
}

//親子付け
void Object3d::Compose(const WorldTransform* parent) {
	worldTransform_->Compose(parent);
}

//親子関係を解除
void Object3d::Decompose() {
	worldTransform_->Decompose();
}

//モデルのセッター
void Object3d::SetModel(const std::string& name) {
	model_ = ModelManager::GetInstance()->FindModel(name);
}

//カメラのセッター
void Object3d::SetCamera(Camera* camera) {
	worldTransform_->SetCamera(camera);
	Object3dCommon::GetInstance()->SetCameraForGPU(camera->GetTranslate());
}

// スケールのセッター
void Object3d::SetScale(const Vector3& scale) {
	worldTransform_->SetScale(scale);
}

// 回転のセッター
void Object3d::SetRotate(const Vector3& rotate) {
	worldTransform_->SetRotate(rotate);
}

// 平行移動のセッター
void Object3d::SetTranslate(const Vector3& translate) {
	worldTransform_->SetTranslate(translate);
}

//トランスフォームのセッター
void Object3d::SetTransform(const TransformData& transform) {
	worldTransform_->SetTransform(transform);
}

// uvスケールのセッター
void Object3d::SetUVScale(const Vector2& uvScale) {
	uvTransform_.scale = uvScale;
}

// uv回転のセッター
void Object3d::SetUVRotate(float uvRotate) {
	uvTransform_.rotate = uvRotate;
}

// uv平行移動のセッター
void Object3d::SetUVTranslate(const Vector2& uvTranslate) {
	uvTransform_.translate = uvTranslate;
}

//色のセッター
void Object3d::SetColor(const Vector4& color) {
	if (model_) {
		model_->SetColor(color);
	}
}

//親のセッター
void Object3d::SetParent(const WorldTransform* parent) {
	worldTransform_->SetParent(parent);
}

//テクスチャの変更
void Object3d::SetTexture(const std::string& filePath) {
	if (model_) {
		model_->SetTexture(filePath);
	}
}

//ブレンドモードのセッター
void Object3d::SetBlendMode(const BlendMode& blendMode) {
	blendMode_ = blendMode;
}

// スケールのゲッター
const Vector3& Object3d::GetScale() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_->GetScale();
}

// 回転のゲッター
const Vector3& Object3d::GetRotate() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_->GetRotate();
}

// 平行移動のゲッター
const Vector3& Object3d::GetTranslate() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_->GetTranslate();
}

// uvスケールのゲッター
const Vector2& Object3d::GetUVScale() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.scale;
}

// uv回転のゲッター
const float Object3d::GetUVRotate() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.rotate;
}

// uv平行移動のゲッター
const Vector2& Object3d::GetUVTranslate() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.translate;
}

//色のゲッター
const Vector4& Object3d::GetColor() const {
	// TODO: return ステートメントをここに挿入します
	static const Vector4 defaultColor(0, 0, 0, 0);
	if (model_) {
		return model_->GetColor();
	}
	return defaultColor;
}

//ワールド行列のゲッター
const WorldTransform* Object3d::GetWorldTransform() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_;
}

//トランスフォームデータのゲッター
const TransformData& Object3d::GetTransformData() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_->GetTransform();
}

//モデルのゲッター
Model* Object3d::GetModel() {
	if (model_) {
		return model_;
	}
	return nullptr;
}
