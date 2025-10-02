#include "TitleScene.h"
#include "engine/input/Input.h"
#include "SceneManager.h"
#include "SceneFactory.h"
#include "engine/3d/ModelManager.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/2d/TextureManager.h"
#include "engine/math/func/Math.h"
#include "engine/particle/ParticleManager.h"

//初期化
void TitleScene::Initialize(DirectXBase* directXBase) {
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize();
	object3d_->SetModel("cube");
	transformData3d_.translate = { 0.0f,0.0f,10.0f };

	//デバッグカメラ
	debugCamera_ = CameraManager::GetInstance()->FindCamera("debugCamera");

	object3d_->SetCamera(debugCamera_);

	//入力
	input_ = Input::GetInstance();
	input_->Initialize();
}

//更新
void TitleScene::Update() {
	object3d_->SetTransform(transformData3d_);
	object3d_->Update();

	debugCamera_->SetRotate(cameraRotate_);
	debugCamera_->SetTranslate(cameraTranslate_);
	debugCamera_->Update();

	//カメラの移動
	//横移動
	if (input_->PressKey(DIK_A)) {
		cameraMoveDir.x = -1.0f;
	}
	else if (input_->PressKey(DIK_D)) {
		cameraMoveDir.x = 1.0f;
	}
	else {
		cameraMoveDir.x = 0.0f;
	}

	//前後移動
	if (input_->PressKey(DIK_W)) {
		cameraMoveDir.z = 1.0f;
	}
	else if (input_->PressKey(DIK_S)) {
		cameraMoveDir.z = -1.0f;
	}
	else {
		cameraMoveDir.z = 0.0f;
	}

	//上下移動
	if (input_->PressKey(DIK_Q)) {
		cameraMoveDir.y = 1.0f;
	}
	else if (input_->PressKey(DIK_E)) {
		cameraMoveDir.y = -1.0f;
	}
	else {
		cameraMoveDir.y = 0.0f;
	}

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Rendering::MakeRotateXYZMatrix(cameraRotate_);

	//カメラの向いてる方向を正にする
	cameraMoveDir = Math::TransformNormal(cameraMoveDir, rotMat);

	//カメラを移動させる
	cameraTranslate_ += cameraMoveDir * cameraSpeed_;

	//マウスのフリックを取得
	if (input_->PressMouseButton(kLeft)) {
		Vector2Int mouseFlick = input_->GetMouseMoveAmount();
		//フリックの値をVector2に格納
		cameraFlickVector_.x = static_cast<float>(mouseFlick.x);
		cameraFlickVector_.y = static_cast<float>(mouseFlick.y);

		//フリックの値をカメラの回転に反映
		cameraRotate_.x += cameraFlickVector_.y / 1000.0f;
		cameraRotate_.y += cameraFlickVector_.x / 1000.0f;
	}

#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();

	ImGui::Begin("object3d");
	ImGui::DragFloat3("rotate", &transformData3d_.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &transformData3d_.translate.x, 0.1f);
	ImGui::End();

	ImGui::Begin("camera");
	ImGui::DragFloat3("rotate", &cameraRotate_.x, 0.1f);
	ImGui::DragFloat3("translate", &cameraTranslate_.x, 0.1f);
	ImGui::DragFloat("speed", &cameraSpeed_, 0.1f);
	ImGui::Text("flick(x:%f,y:%f)", cameraFlickVector_.x, cameraFlickVector_.y);
	ImGui::End();

	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI

}

//描画
void TitleScene::Draw() {
	object3d_->Draw();
}

//終了
void TitleScene::Finalize() {

}
