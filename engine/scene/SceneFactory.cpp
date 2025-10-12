#include "SceneFactory.h"
#include "application/scene/TitleScene.h"
#include "application/scene/GameScene.h"
// シーンの生成
IScene* SceneFactory::CreateScene(const std::string& sceneName) {
	//次のシーンの生成
	IScene* newScene = nullptr;
	if (sceneName == "Title") {
		newScene = new TitleScene();
	} else if (sceneName == "Game") {
		newScene = new GameScene();
	}
	return newScene;
}
