#include "ControlPoint.h"
#include "engine/3d/Object3d.h"
#include "engine/debug/GlobalVariables.h"

//初期化
void ControlPoint::Initialize(Camera* camera) {
	//カメラを記録
	camera_ = camera;
	//制御ポイントの初期化
	//配列のサイズを設定
	//controlPointDataList_.resize(kMaxPointCount);
	//for (ControlPointData& data : controlPointDataList_) {
	//	//色
	//	data.color = Vector4::MakeWhiteColor();
	//	//位置
	//	data.point = {};
	//	//大きさ
	//	data.scale = Vector3::MakeAllOne() / 2.0f;
	//	////3dオブジェクト
	//	//data.object3d = new Object3d();
	//	//data.object3d->Initialize(camera_);
	//	//data.object3d->SetModel("sphere");
	//}

	////グローバル変数
	//GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//globalVariables->AddItem(groupName_, "point0", controlPointDataList_[0].point);
	//globalVariables->AddItem(groupName_, "point1", controlPointDataList_[1].point);
	//globalVariables->AddItem(groupName_, "point2", controlPointDataList_[2].point);
	//globalVariables->AddItem(groupName_, "point3", controlPointDataList_[3].point);
	//globalVariables->AddItem(groupName_, "point4", controlPointDataList_[4].point);
}

//更新
void ControlPoint::Update() {
	////グローバル変数に適応
	//ApplyGlobalVariables();
	////全配列にアクセス
	//for (ControlPointData& data : controlPointDataList_) {
	//	//位置の設定
	//	data.object3d->SetTranslate(data.point);
	//	//大きさの設定
	//	data.object3d->SetScale(data.scale);
	//	//3dオブジェクトの更新
	//	data.object3d->Update();
	//}
}

//グローバル変数に適応
void ControlPoint::ApplyGlobalVariables() {
	/*GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	controlPointDataList_[0].point = globalVariables->GetValue<Vector3>(groupName_, "point0");
	controlPointDataList_[1].point = globalVariables->GetValue<Vector3>(groupName_, "point1");
	controlPointDataList_[2].point = globalVariables->GetValue<Vector3>(groupName_, "point2");
	controlPointDataList_[3].point = globalVariables->GetValue<Vector3>(groupName_, "point3");
	controlPointDataList_[4].point = globalVariables->GetValue<Vector3>(groupName_, "point4");*/
}

//描画
void ControlPoint::Draw() {
	//for (ControlPointData& data : controlPointDataList_) {
	//	//描画
	//	data.object3d->Draw();
	//}
}

//終了
void ControlPoint::Finalize() {
	//for (ControlPointData& data : controlPointDataList_) {
	//	delete data.object3d;
	//	data.object3d = nullptr;
	//}
	//controlPointDataList_.clear();
}

//カメラのセッター
void ControlPoint::SetCamera(Camera* camera) {
	//camera_ = camera;
	//for (ControlPointData& data : controlPointDataList_) {
	//	data.object3d->SetCamera(camera_);
	//}
}

//制御ポイントのゲッター
std::vector<Vector3> ControlPoint::GetControlPoints() {
	//std::vector<Vector3>result;
	//result.resize(controlPointDataList_.size());
	//for (int32_t i = 0; i < controlPointDataList_.size(); i++) {
	//	result[i] = controlPointDataList_[i].point;
	//}
	//return result;
	return{};
}
