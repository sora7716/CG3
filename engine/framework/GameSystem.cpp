#include "GameSystem.h"
#include "engine/scene/SceneManager.h"
#include "engine/scene/SceneFactory.h"
#include "application/actor/EnemyManager.h"
//初期化
void GameSystem::Initialize() {
	Framework::Initialize();
	//シーンファクトリーの生成
	sceneFactory_ = new SceneFactory();
	//シーンファクトリーのセット
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);
	//タイトルシーンを呼び出す
	SceneManager::GetInstance()->ChangeScene("Title");
#ifdef _DEBUG
	//シーンの管理
	SceneManager::GetInstance()->Update();
	//デバッグしたいシーンを呼び出す
	SceneManager::GetInstance()->ChangeScene("Game");
#endif // _DEBUG
}

//更新
void GameSystem::Update() {
	Framework::Update();
}

//描画
void GameSystem::Draw() {
	//描画開始位置
	core_->GetDirectXBase()->PreDraw();
	//SRVの管理
	SRVManager::GetInstance()->PreDraw();
	//シーン
	SceneManager::GetInstance()->Draw();
	//ImGuiの管理
	ImGuiManager::GetInstance()->Draw();
	//描画終了位置
	core_->GetDirectXBase()->PostDraw();
}

//終了
void GameSystem::Finalize() {
	//敵の解放
	EnemyManager::GetInstance()->Finalize();

	Framework::Finalize();
}
