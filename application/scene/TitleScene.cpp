#include "TitleScene.h"
#include "engine/input/Input.h"
#include "engine/scene/SceneManager.h"
#include "engine/math/func/Collision.h"

//初期化
void TitleScene::Initialize(DirectXBase* directXBase) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase);
	camera_ = CameraManager::GetInstance()->FindCamera("titleCamera");

	for (uint32_t i = 0; i < wireframeObjects_.size(); i++) {
		wireframeObjects_[i] = std::make_unique<WireframeObject3d>();
	}
	wireframeObjects_[0]->Initialize(camera_, ModelType::kSphere);
	wireframeObjects_[1]->Initialize(camera_, ModelType::kCube);

	transformData_[0].scale = Vector3::MakeAllOne();
	transformData_[0].translate = { -2.0f,0.0f,2.0f };
	transformData_[1].scale = Vector3::MakeAllOne();
	transformData_[1].translate = { 2.0f,1.0f,0.0f };
}

//更新ww
void TitleScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	//カメラの設定
	for (uint32_t i = 0; i < wireframeObjects_.size(); i++) {
		wireframeObjects_[i]->SetCamera(camera_);
		wireframeObjects_[i]->SetRotate(transformData_[i].rotate);
		wireframeObjects_[i]->SetTranslate(transformData_[i].translate);
		wireframeObjects_[i]->Update();
	}
	wireframeObjects_[0]->SetRadius(radius_[0]);
	wireframeObjects_[1]->SetScale(transformData_[1].scale);

	//衝突判定
	if (Collision::IsCollision(wireframeObjects_[1]->GetOBB(), wireframeObjects_[0]->GetSphere())) {
		wireframeObjects_[0]->SetColor(Vector4::MakeRedColor());
		wireframeObjects_[1]->SetColor(Vector4::MakeRedColor());
	} else {
		wireframeObjects_[0]->SetColor(Vector4::MakeBlackColor());
		wireframeObjects_[1]->SetColor(Vector4::MakeBlackColor());
	}

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	ImGui::Begin("wireframeObject");
	for (uint32_t i = 0; i < wireframeObjects_.size(); i++) {
		ImGui::Separator();
		ImGui::PushID(i);
		ImGuiManager::GetInstance()->DragTransform(transformData_[i]);
		ImGui::DragFloat("radius", &radius_[i], 0.1f);
		ImGui::PopID();
	}
	ImGui::End();
	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = CameraManager::GetInstance()->FindCamera("titleCamera");
	}
#endif // _DEBUG


}

//描画
void TitleScene::Draw() {
	for (uint32_t i = 0; i < wireframeObjects_.size(); i++) {
		wireframeObjects_[i]->Draw();
	}
}

//終了
void TitleScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
