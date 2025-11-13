#include "RailCamera.h"
#include "engine/camera/Camera.h"
#include "engine/debug/GlobalVariables.h"
#include "engine/worldTransform/WorldTransform.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/3d/Object3d.h"

//初期化
void RailCamera::Initialize(Camera* camera, const Vector3& rotate, const Vector3& position){
	//カメラを記録
	camera_ = camera;

	//カメラのオブジェクト
	cameraObject_ = new Object3d();
	cameraObject_->Initialize(camera_);
	cameraObject_->SetModel("bullet");

	//回転を記録
	rotate_ = rotate;
	//位置を記録
	translate_ = position;
	//調整項目
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//調整項目のグループの生成
	globalVariables->CreateGroup(groupName_);
	globalVariables->AddItem(groupName_, "rotate", rotate_);
	globalVariables->AddItem(groupName_, "translate", translate_);
}

//更新
void RailCamera::Update() {
	//調整項目を適応
	ApplyGlobalVariables();

	camera_->SetRotate(rotate_);
	camera_->SetTranslate(translate_);
	//カメラのオブジェクト
	cameraObject_->SetRotate(rotate_);
	cameraObject_->SetTranslate(translate_);
	cameraObject_->Update();
}

//描画
void RailCamera::Draw() {
	cameraObject_->Draw();
}

//終了
void RailCamera::Finalize() {
	delete cameraObject_;
}

//調整項目の適応
void RailCamera::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//回転の取得
	rotate_ = globalVariables->GetValue<Vector3>(groupName_, "rotate");
	//位置の取得
	translate_ = globalVariables->GetValue<Vector3>(groupName_, "translate");
}

//ワールドトランスフォームのゲッター
const WorldTransform* RailCamera::GetWorldTransform() const{
	return cameraObject_->GetWorldTransform();
}

//カメラのゲッター
Camera* RailCamera::GetCamera() {
	return camera_;
}

void RailCamera::SetCamera(Camera* camera) {
	cameraObject_->SetCamera(camera);
}