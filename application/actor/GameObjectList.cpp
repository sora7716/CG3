#include "GameObjectList.h"
#include <cassert>

//インスタンスのゲッター
GameObjectList* GameObjectList::GetInstance() {
	assert(!isFinalize && "GetInstance() called after Finalize()");
	if (instance == nullptr) {
		instance = new GameObjectList();
	}
	return instance;
}

//初期化
void GameObjectList::Initialize() {
	//オーディオの読み込み
	LoadAudio();
	//OBJファイルの読み込み
	LoadModel();
	//カメラの生成
	CreateCamera();
}

//終了
void GameObjectList::Finalize() {
	delete instance;
	instance = nullptr;
	isFinalize = true;
}


//オーディオの読み込み
void GameObjectList::LoadAudio() {
	AudioManager::GetInstance()->LoadAudio("Alarm01", "Alarm01");
	AudioManager::GetInstance()->LoadAudio("mokugyo", "mokugyo");
}

//OBJファイルの読み込み
void GameObjectList::LoadModel() {
	//モデルの読み込み
	//プレイヤー
	ModelManager::GetInstance()->LoadOBJModel("player", "sphere", "sphere");
	//球
	ModelManager::GetInstance()->LoadOBJModel("sphere", "sphere", "sphere");
	//弾
	ModelManager::GetInstance()->LoadOBJModel("bullet", "sphere", "sphere");
	//カメラ
	ModelManager::GetInstance()->LoadOBJModel("camera", "camera", "camera");
	//地面
	ModelManager::GetInstance()->LoadOBJModel("ground", "cube", "cube");
	//フィールド
	ModelManager::GetInstance()->LoadOBJModel("field", "terrain", "terrain");
	//球
	ModelManager::GetInstance()->LoadOBJModel("sphere", "sphere", "sphere");
	//平面
	ModelManager::GetInstance()->LoadOBJModel("plane", "plane", "plane");
	//スポットライト
	ModelManager::GetInstance()->LoadOBJModel("spotLight", "plane", "plane");
	//軸
	ModelManager::GetInstance()->LoadOBJModel("test", "base", "axis");
}

//カメラの生成
void GameObjectList::CreateCamera() {
	//カメラの管理
	CameraManager::GetInstance()->CreateCamera("defaultCamera");
	//デバッグカメラ
	CameraManager::GetInstance()->CreateCamera("debugCamera");
	//タイトルカメラ
	CameraManager::GetInstance()->CreateCamera("titleCamera");
	//ゲームカメラ
	CameraManager::GetInstance()->CreateCamera("gameCamera");
}