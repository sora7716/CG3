#include "Core.h"
//初期化
void Core::Initialize() {
	//WinApi
	winApi_ = std::make_unique<WinApi>(WinApi::ConstructorKey{});
	winApi_->Initialize();
	//DirectXの基盤部分
	directXBase_ = std::make_unique<DirectXBase>(DirectXBase::ConstructorKey{});
	directXBase_->Initialize(winApi_.get());
	//カメラマナージャー
	cameraManager_ = std::make_unique<CameraManager>(CameraManager::ConstructorKey{});
}

//WinApiのゲッター
WinApi* Core::GetWinApi() {
	return winApi_.get();
}

//DirectXの基盤部分
DirectXBase* Core::GetDirectXBase() {
	return directXBase_.get();
}

//カメラマネージャーのゲッター
CameraManager* Core::GetCameraManager() {
	return cameraManager_.get();
}
