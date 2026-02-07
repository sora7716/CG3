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
	//テクスチャマネージャー
	textureManager_ = std::make_unique<TextureManager>(TextureManager::ConstructorKey{});
	textureManager_->Initialize(directXBase_.get(), srvManager_.get());
	//モデルマネージャー
	modelManager_ = std::make_unique<ModelManager>(ModelManager::ConstructorKey{});
	modelManager_->Initialize(directXBase_.get(), textureManager_.get());
	//ImGuiマネージャー
	imguiManager_ = std::make_unique<ImGuiManager>(ImGuiManager::ConstructorKey{});
	imguiManager_->Initialize(winApi_.get(), directXBase_.get(), srvManager_.get());
	//カメラマナージャー
	cameraManager_ = std::make_unique<CameraManager>(CameraManager::ConstructorKey{});
	//スプライトの共通部分
	spriteCommon_ = std::make_unique<SpriteCommon>(SpriteCommon::ConstructorKey{});
	spriteCommon_->Initialize(directXBase_.get(), textureManager_.get());
	//2Dオブジェクトの共通部分
	object2dCommon_ = std::make_unique<Object2dCommon>(Object2dCommon::ConstructorKey{});
	object2dCommon_->Initialize(directXBase_.get(), textureManager_.get());
	//3Dオブジェクトの共通部分
	object3dCommon_ = std::make_unique<Object3dCommon>(Object3dCommon::ConstructorKey{});
	object3dCommon_->Initialize(directXBase_.get(), srvManager_.get(), textureManager_.get(), modelManager_.get());
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

//テクスチャマネージャーのゲッター
TextureManager* Core::GetTextureManager() const {
	return textureManager_.get();
}

//モデルマネージャー
ModelManager* Core::GetModelManager() const {
	return modelManager_.get();
}

//ImGuiマネージャーのゲッター
ImGuiManager* Core::GetImGuiManager() const {
	return imguiManager_.get();
}

//カメラマネージャーのゲッター
CameraManager* Core::GetCameraManager()const {
	return cameraManager_.get();
}

//スプライトの共通部分のゲッター
SpriteCommon* Core::GetSpriteCommon() const {
	return spriteCommon_.get();
}

//2Dオブジェクトの共通部分
Object2dCommon* Core::GetObject2dCommon() const {
	return object2dCommon_.get();
}

//3Dオブジェクトの共通部分のゲッター
Object3dCommon* Core::GetObject3dCommon() const {
	return object3dCommon_.get();
}
