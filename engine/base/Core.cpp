#include "Core.h"
//初期化
void Core::Initialize() {
	//WinApi
	winApi_ = std::make_unique<WinApi>(WinApi::ConstructorKey{});
	winApi_->Initialize();
	//DirectXの基盤部分
	directXBase_ = std::make_unique<DirectXBase>(DirectXBase::ConstructorKey{});
	directXBase_->Initialize(winApi_.get());
	//SRVマネージャー
	srvManager_ = std::make_unique<SRVManager>(SRVManager::ConstructorKey{});
	srvManager_->Initialize(directXBase_.get());
	//入力
	input_ = std::make_unique<Input>(Input::ConstructorKey{});
	input_->Initialize(winApi_.get());
	//カメラマナージャー
	cameraManager_ = std::make_unique<CameraManager>(CameraManager::ConstructorKey{});
}

//WinApiのゲッター
WinApi* Core::GetWinApi()const {
	return winApi_.get();
}

//DirectXの基盤部分
DirectXBase* Core::GetDirectXBase()const {
	return directXBase_.get();
}

//SRVマネージャーのゲッター
SRVManager* Core::GetSRVManager()const {
	return srvManager_.get();
}

//入力のゲッター
Input* Core::GetInput()const {
	return input_.get();
}

//カメラマネージャーのゲッター
CameraManager* Core::GetCameraManager()const {
	return cameraManager_.get();
}
