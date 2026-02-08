#include "IScene.h"
#include "engine/scene/SceneManager.h"
#include "engine/scene/SceneFactory.h"
#include "engine/base/Core.h"
#include "engine/debug/GlobalVariables.h"

//初期化
void IScene::Initialize(Core* core) {
	//ゲームエンジンの核
	core_ = core;
	//デバックカメラ
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(core_->GetInput(), core_->GetCameraManager());
	////調整ファイルの読み込み
	//GlobalVariables::GetInstance()->LoadFiles();
}

//更新
void IScene::Update() {
	//デバックカメラ
	debugCamera_->Update();
}

//終了
void IScene::Finalize() {
	//シーンファクトリーの解放
	delete sceneFactory_;
	sceneFactory_ = nullptr;
}
