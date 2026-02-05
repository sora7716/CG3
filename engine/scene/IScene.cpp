#include "IScene.h"
#include "engine/scene/SceneManager.h"
#include "engine/scene/SceneFactory.h"
#include "engine/debug/GlobalVariables.h"

//初期化
void IScene::Initialize(DirectXBase* directXBase, CameraManager* cameraManager) {
	//DirectXBaseを受け取る
	directXBase_ = directXBase;
	//カメラマネージャーを受け取る
	cameraManager_ = cameraManager;
	//デバックカメラ
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(cameraManager_);
	//シーンファクトリーの生成
	sceneFactory_ = new SceneFactory();
	//シーンファクトリーのセット
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);
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