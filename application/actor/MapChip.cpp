#include "MapChip.h"
#include "engine/3d/Object3dCommon.h"
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
MapChip::~MapChip() {
	delete worldTransform_;
}

//初期化
void MapChip::Initialize() {
	//DirectXの基盤部分を受け取る
	directXBase_ = Object3dCommon::GetInstance()->GetDirectXBase();

	//ワールドトランスフォームの生成、初期化
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize(directXBase_, TransformMode::k3d);

	//uv座標
	uvTransform_ = { {1.0f,1.0f},0.0f,{0.0f,0.0f} };

	//カメラにデフォルトカメラを代入
	worldTransform_->SetCamera(Object3dCommon::GetInstance()->GetDefaultCamera());
	//カメラをセット
	Object3dCommon::GetInstance()->CreateCameraResource(worldTransform_->GetCamera()->GetTranslate());

	//マテリアルの初期化
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.uvMatrix = Matrix4x4::Identity4x4();
	material_.shininess = 10.0f;
}


//更新
void MapChip::Update() {
	//Object3dの共通部分の更新
	Object3dCommon::GetInstance()->Update();

	//ワールドトランスフォーム
	worldTransform_->Update();
	if (model_) {
		model_->UVTransform(uvTransform_);
	}
}

//描画
void MapChip::Draw() {
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
void MapChip::Compose(const WorldTransform* parent) {
	worldTransform_->Compose(parent);
}

//親子関係を解除
void MapChip::Decompose() {
	worldTransform_->Decompose();
}

//モデルのセッター
void MapChip::SetModel(const std::string& name) {
	model_ = ModelManager::GetInstance()->FindModel(name);
}

//カメラのセッター
void MapChip::SetCamera(Camera* camera) {
	worldTransform_->SetCamera(camera);
	Object3dCommon::GetInstance()->SetCameraForGPU(camera->GetTranslate());
}

// スケールのセッター
void MapChip::SetScale(const Vector3& scale) {
	worldTransform_->SetScale(scale);
}

// 回転のセッター
void MapChip::SetRotate(const Vector3& rotate) {
	worldTransform_->SetRotate(rotate);
}

// 平行移動のセッター
void MapChip::SetTranslate(const Vector3& translate) {
	worldTransform_->SetTranslate(translate);
}

//トランスフォームのセッター
void MapChip::SetTransform(const TransformData& transform) {
	worldTransform_->SetTransform(transform);
}

// uvスケールのセッター
void MapChip::SetUVScale(const Vector2& uvScale) {
	uvTransform_.scale = uvScale;
}

// uv回転のセッター
void MapChip::SetUVRotate(const float uvRotate) {
	uvTransform_.rotate = uvRotate;
}

// uv平行移動のセッター
void MapChip::SetUVTranslate(const Vector2& uvTranslate) {
	uvTransform_.translate = uvTranslate;
}

//色のセッター
void MapChip::SetColor(const Vector4& color) {
	if (model_) {
		model_->SetColor(color);
	}
}

//親のセッター
void MapChip::SetParent(const WorldTransform* parent) {
	worldTransform_->SetParent(parent);
}

//テクスチャの変更
void MapChip::SetTexture(const std::string& filePath) {
	if (model_) {
		model_->SetTexture(filePath);
	}
}

//ブレンドモードのセッター
void MapChip::SetBlendMode(const BlendMode& blendMode) {
	blendMode_ = blendMode;
}

// スケールのゲッター
const Vector3& MapChip::GetScale() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_->GetScale();
}

// 回転のゲッター
const Vector3& MapChip::GetRotate() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_->GetRotate();
}

// 平行移動のゲッター
const Vector3& MapChip::GetTranslate() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_->GetTranslate();
}

// uvスケールのゲッター
const Vector2& MapChip::GetUVScale() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.scale;
}

// uv回転のゲッター
const float MapChip::GetUVRotate() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.rotate;
}

// uv平行移動のゲッター
const Vector2& MapChip::GetUVTranslate() const {
	// TODO: return ステートメントをここに挿入します
	return uvTransform_.translate;
}

//色のゲッター
const Vector4& MapChip::GetColor() const {
	// TODO: return ステートメントをここに挿入します
	static const Vector4 defaultColor(0, 0, 0, 0);
	if (model_) {
		return model_->GetColor();
	}
	return defaultColor;
}

//ワールド行列のゲッター
const WorldTransform* MapChip::GetWorldTransform() const {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_;
}

//モデルのゲッター
Model* MapChip::GetModel() {
	if (model_) {
		return model_;
	}
	return nullptr;
}
