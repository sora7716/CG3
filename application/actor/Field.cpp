#include "Field.h"
#include "engine/camera/Camera.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/debug/GlobalVariables.h"
#include "MapChip.h"
#include <cmath>

//初期化
void Field::Initialize(Camera* camera) {
	mapChip_ = new MapChip();
	mapChip_->Initialize(Object3dCommon::GetInstance()->GetDirectXBase(), camera, MapChipType::kBlock, "map.csv", "ground", -2.0f, mapSize_);

	//調整項目
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//調整項目のグループの生成
	globalVariables->CreateGroup(groundGroupName_);
	globalVariables->AddItem(groundGroupName_, "color",mapChip_->GetMaterial().color);
	globalVariables->AddItem(groundGroupName_, "enableLighting",mapChip_->GetMaterial().enableLighting);
	globalVariables->AddItem(groundGroupName_, "shininess",mapChip_->GetMaterial().shininess);
}

//更新
void Field::Update() {
	//調整項目を適応
	ApplyGlobalVariables();
	//マップチップの更新
	mapChip_->Update();
}

//描画
void Field::Draw() {
	//マップチップの描画
	mapChip_->Draw();
}

//終了
void Field::Finalize() {
	mapChip_->Finalize();
	delete mapChip_;
	mapChip_ = nullptr;
}

//調整項目を適応
void Field::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//地面のマテリアル
	Material ground = mapChip_->GetMaterial();
	ground.color = globalVariables->GetValue<Vector4>(groundGroupName_, "color");
	ground.enableLighting = globalVariables->GetValue<int32_t>(groundGroupName_, "enableLighting");
	ground.shininess = globalVariables->GetValue<float>(groundGroupName_, "shininess");
	mapChip_->SetMaterial(ground);
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