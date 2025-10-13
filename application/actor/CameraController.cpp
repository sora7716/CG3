#include "CameraController.h"
#include "engine/camera/CameraManager.h"
#include "application/actor/Player.h"
#include "engine/math/RenderingData.h"

//初期化
void CameraController::Initialize(const std::string& cameraName) {
	//カメラを受け取る
	camera_ = CameraManager::GetInstance()->FindCamera(cameraName);
}

//更新
void CameraController::Update() {
	//追従対象のトランスフォームデータを参照
	TransformData targetTransformData = target_->GetTransformData();
	//追従対象とオフセットからカメラの座標を計算
	camera_->SetTranslate(targetTransformData.translate + targetOffset_);
	//行列を更新
	camera_->Update();
}

//追従対象
void CameraController::SetTarget(Player* target) {
	target_ = target;
}

//リセット
void CameraController::Reset() {
	//追従対象のトランスフォームデータを参照
	TransformData targetTransformData = target_->GetTransformData();
	//追従対象とオフセットからカメラの座標を計算
	camera_->SetTranslate(targetTransformData.translate + targetOffset_);
}
