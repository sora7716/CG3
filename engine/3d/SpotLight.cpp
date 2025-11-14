#include "SpotLight.h"
#include "Object3d.h"
#include "engine/camera/Camera.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/math/func/Math.h"

//初期化
void SpotLight::Initialize(Camera*camera) {
	object3d_ = new Object3d();
	object3d_->Initialize(camera,TransformMode::kBilboard);
	object3d_->SetModel("spotLight");
	object3d_->SetTexture("spotLight.png");

	//スポットライトの初期化
	spotLightData_.color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData_.position = { 2.0f,1.25f,0.0f };
	spotLightData_.distance = 7.0f;
	spotLightData_.direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
	spotLightData_.intensity = 4.0f;
	spotLightData_.decay = 2.0f;
	spotLightData_.cosAngle = std::cos(Math::kPi / 3.0f);
	spotLightData_.cosFolloffStart = 1.0f;
	spotLightData_.isBlinnPhong = true;
	spotLightData_.enableSpotLighting = true;

	//マテリアルの初期化
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = false;
	material_.shininess = 10.0f;
	material_.uvMatrix = Matrix4x4::Identity4x4();
}

//更新
void SpotLight::Update() {
	object3d_->SetTranslate(spotLightData_.position);
	object3d_->GetModel()->SetMaterial(material_);
	object3d_->Update();
}


//デバッグ用
void SpotLight::Debug() {
	ImGui::DragFloat3("translate", &spotLightData_.position.x, 0.1f);
	ImGui::DragFloat("cosAngle", &spotLightData_.cosAngle, 0.1f);
	ImGui::DragFloat("cosFolloffStart", &spotLightData_.cosFolloffStart, 0.1f);
}

//描画
void SpotLight::Draw() {
#ifdef _DEBUG
	object3d_->Draw();
#endif // _DEBUG
}

//終了
void SpotLight::Finalize() {
	delete object3d_;
	object3d_ = nullptr;
}

//カメラのセッター
void SpotLight::SetCamera(Camera* camera) {
	object3d_->SetCamera(camera);
}

//スポットライトデータのゲッター
SpotLightData& SpotLight::GetSpotLightData() {
	// TODO: return ステートメントをここに挿入します
	return spotLightData_;
}
