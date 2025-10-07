#include "DebugCamera.h"
#include "engine/input/Input.h"
#include "engine/camera/CameraManager.h"
#include "engine/math/func/Math.h"
#include "ImGuiManager.h"
#include <algorithm>

//初期化
void DebugCamera::Initialize() {
	//入力
	input_ = Input::GetInstance();
	input_->Initialize();

	//カメラ
	camera_ = CameraManager::GetInstance()->FindCamera("debugCamera");

	//fovYの設定
	fovY_ = camera_->GetFovY();
}

//更新
void DebugCamera::Update() {
	//平行移動の更新
	TranslateUpdate();

	//回転の操作
	RotateControl();

	//ズーム操作
	ZoomControl();

	//カメラ
	camera_->SetRotate(rotate_);
	camera_->SetTranslate(translate_);
	camera_->Update();
}

//カメラのゲッター
Camera* DebugCamera::GetCamera() {
	return camera_;
}

//デバックに使用する
void DebugCamera::Debug() {
#ifdef USE_IMGUI
	ImGui::DragFloat3("rotate", &rotate_.x, 0.1f);
	ImGui::DragFloat2("flick", &mouseFlick_.x, 0.1f);
	ImGui::DragFloat("fovY", &fovY_, 0.1f);
	ImGui::DragFloat("speed", &speed_, 0.01f);
#endif // USE_IMGUI
}

//左右移動の操作
void DebugCamera::StrafeControl() {
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

//上下移動の操作
void DebugCamera::ElevateControl() {
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

//前後移動の操作
void DebugCamera::DollyControl() {
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

//ズーム操作
void DebugCamera::ZoomControl() {
	//マウスホイールの回転量でズームイン、ズームアウト

	fovY_ += static_cast<float>(input_->GetWheelRotate()) * kZoomSpeedMagnification;
	fovY_ = std::clamp(fovY_, kMinFovY, kMaxFovY);
	//fovYのセット
	camera_->SetFovY(fovY_);

	//ズーム操作のリセット
	if (input_->PressMouseButton(kMiddle)) {
		fovY_ = 0.45f;
	}
}

//回転の操作
void DebugCamera::RotateControl() {
	//マウスのフリックを取得
	if (input_->PressMouseButton(kRight)) {
		Vector2Int mouseFlick = input_->GetMouseMoveAmount();
		//フリックの値をVector2に格納
		mouseFlick_.x = static_cast<float>(mouseFlick.x);
		mouseFlick_.y = static_cast<float>(mouseFlick.y);

		//フリックの値をカメラの回転に反映
		rotate_.x += mouseFlick_.y * kLookRadPerCount;
		rotate_.y += mouseFlick_.x * kLookRadPerCount;
	}
}

//平行移動の更新
void DebugCamera::TranslateUpdate() {
	//X軸方向の移動
	StrafeControl();

	//Y軸方向の移動
	ElevateControl();

	//Z軸方向の移動
	DollyControl();

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Rendering::MakeRotateXYZMatrix(rotate_);

	//カメラの向いてる方向を正にする(XとZ軸限定)
	Vector3 moveDirXZ = Math::TransformNormal(Vector3(moveDir_.x, 0.0f, moveDir_.z), rotMat);

	//Y軸のそのまま
	moveDir_ = { moveDirXZ.x,moveDir_.y,moveDirXZ.z };

	//カメラを移動させる
	translate_ += moveDir_.Normalize() * speed_;
}