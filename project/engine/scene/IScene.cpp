#include "IScene.h"
#include "engine/debug/DebugCamera.h"
#include "engine/scene/AbstractSceneFactory.h"
#include "engine/debug/GlobalVariables.h"

//コンストラクタ
IScene::IScene() {
}

//デストラクタ
IScene::~IScene() {
}

//初期化
void IScene::Initialize(const SceneContext& sceneContext) {
	//ゲームエンジンの核
	sceneContext_ = sceneContext;
	//デバックカメラ
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(sceneContext_.input, sceneContext_.cameraManager);
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
