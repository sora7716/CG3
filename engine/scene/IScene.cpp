#include "IScene.h"
#include "engine/scene/SceneManager.h"
#include "engine/scene/SceneFactory.h"

//初期化
void IScene::Initialize(DirectXBase* directXBase) {
	//DirectXBaseを受け取る
	directXBase_ = directXBase;
	//デバックカメラ
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	//シーンファクトリーの生成
	sceneFactory_ = new SceneFactory();
	//シーンファクトリーのセット
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);
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