#include "GameCamera.h"
#include "engine/camera/Camera.h"
#include "engine/debug/ImGuiManager.h"

//初期化
void GameCamera::Initialize(Camera* camera, const Vector3& offset) {
	//カメラとオフセットの記録
	camera_ = camera;
	offset_ = offset;
}

//更新
void GameCamera::Update() {
	//カメラの位置をオフセット分離す
	camera_->SetTranslate(targetPos_ + offset_);

	//カメラの回転を設定
	camera_->SetRotate(rotate_);

	//カメラの更新
	camera_->Update();
}

//デバッグ
void GameCamera::Debug() {
	ImGui::DragFloat3("rotate", &rotate_.x, 0.1f);
	ImGui::DragFloat3("offset", &offset_.x, 0.1f);
}

//カメラのゲッター
Camera* GameCamera::GetCamera() {
	return camera_;
}

//カメラのセッター
void GameCamera::SetCamera(Camera* camera) {
	camera_ = camera;
}

//ターゲットの位置のセッター
void GameCamera::SetTragetPos(const Vector3& targetPos) {
	targetPos_ = targetPos;
}
