#include "Field.h"
#include "engine/camera/Camera.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/debug/ImGuiManager.h"
#include "MapChip.h"
#include <cmath>

//初期化
void Field::Initialize(Camera* camera) {
	mapChip_ = new MapChip();
	mapChip_->Initialize(Object3dCommon::GetInstance()->GetDirectXBase(), camera, MapChipType::kBlock, "map.csv", "ground", -2.0f, mapSize_);
}

//更新
void Field::Update() {
	mapChip_->Update();
}

//描画
void Field::Draw() {
	mapChip_->Draw();
}

//終了
void Field::Finalize() {
	mapChip_->Finalize();
	delete mapChip_;
	mapChip_ = nullptr;
}

//デバッグ
void Field::Debug() {
#ifdef USE_IMGUI
	mapChip_->Debug();
#endif // USE_IMGUI
}

//カメラのセッター
void Field::SetCamera(Camera* camera) {
	mapChip_->SetCamera(camera);
}

//テクスチャのセッター
void Field::SetTexture(const std::string& textureName) {
	mapChip_->SetTexture(textureName);
}

//平行光源のセッター
void Field::SetDirectionalLight(const DirectionalLight& directional) {
	mapChip_->SetDirectionalLight(directional);
}

//点光源のセッター
void Field::SetPointLight(const PointLight* point) {
	mapChip_->SetPontLight(point);
}

//スポットライトのセッターk
void Field::SetSpotLight(const SpotLight* spot) {
	mapChip_->SetSpotLight(spot);
}