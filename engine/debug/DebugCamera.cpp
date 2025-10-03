#include "DebugCamera.h"
#include "engine/input/Input.h"
#include "engine/camera/CameraManager.h"
#include "engine/math/func/Math.h"
#include "ImGuiManager.h"

//初期化
void DebugCamera::Initialize() {
	//入力
	input_ = Input::GetInstance();
	input_->Initialize();

	//カメラ
	camera_ = CameraManager::GetInstance()->FindCamera("debugCamera");
}

//更新
void DebugCamera::Update() {
	//入力
	input_->Update();

	//平行移動の更新
	UpdateTranslate();

	//回転の更新
	UpdateRotate();

	//カメラ
	camera_->SetRotate(rotate_);
	camera_->SetTranslate(translate_);
	camera_->Update();
}

//カメラのゲッター
Camera* DebugCamera::GetCamera() {
	return camera_;
}

void DebugCamera::Debug() {
	ImGui::Begin("debugCamera");
	ImGui::DragFloat3("rotate", &rotate_.x, 0.1f);
	ImGui::DragFloat2("flick", &mouseFlick_.x, 0.1f);
	ImGui::End();
}

//X軸ローカル移動
void DebugCamera::TranslateLocalX() {
	//横移動
	if (input_->PressKey(DIK_A)) {
		moveDir_.x = -1.0f;
	}
	else if (input_->PressKey(DIK_D)) {
		moveDir_.x = 1.0f;
	}
	else {
		moveDir_.x = 0.0f;
	}
}

//Y軸ローカル移動
void DebugCamera::TranslateLocalY() {
	if (input_->PressKey(DIK_Q)) {
		moveDir_.y = 1.0f;
	}
	else if (input_->PressKey(DIK_E)) {
		moveDir_.y = -1.0f;
	}
	else {
		moveDir_.y = 0.0f;
	}
}

//Z軸ローカル移動
void DebugCamera::TranslateLocalZ() {
	if (input_->PressKey(DIK_W)) {
		moveDir_.z = 1.0f;
	}
	else if (input_->PressKey(DIK_S)) {
		moveDir_.z = -1.0f;
	}
	else {
		moveDir_.z = 0.0f;
	}

}

//平行移動の更新
void DebugCamera::UpdateTranslate() {
	//X軸方向の移動
	TranslateLocalX();

	//Y軸方向の移動
	TranslateLocalY();

	//Z軸方向の移動
	TranslateLocalZ();

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Rendering::MakeRotateXYZMatrix(rotate_);

	//カメラの向いてる方向を正にする
	moveDir_ = Math::TransformNormal(moveDir_, rotMat);

	//カメラを移動させる
	translate_ += moveDir_ * speed_;
}

//回転の更新
void DebugCamera::UpdateRotate() {
	//マウスのフリックを取得
	if (input_->PressMouseButton(kLeft)) {
		Vector2Int mouseFlick = input_->GetMouseMoveAmount();
		//フリックの値をVector2に格納
		mouseFlick_.x = static_cast<float>(mouseFlick.x);
		mouseFlick_.y = static_cast<float>(mouseFlick.y);

		//フリックの値をカメラの回転に反映
		rotate_.x += mouseFlick_.y * kLookRadPerCount;
		rotate_.y += mouseFlick_.x * kLookRadPerCount;
	}
}
