#include "GameObjectList.h"
#include "engine/base/Core.h"
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
void GameObjectList::Initialize(Core* core) {
	//エンジンの核を記録する
	core_ = core;
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
	
	//デバッグ用
	//球
	ModelManager::GetInstance()->LoadModel("sphere", "sphere", "sphere.gltf");
	//キューブ
	ModelManager::GetInstance()->LoadModel("cube", "cube", "cube.obj");

	//ゲームで使用するモデル
	//プレイヤー
	ModelManager::GetInstance()->LoadModel("player", "sphere", "sphereFlat.gltf");
	//敵
	ModelManager::GetInstance()->LoadModel("enemy", "enemy", "enemy.gltf");
	//球
	ModelManager::GetInstance()->LoadModel("sphere", "sphere", "sphere.gltf");
	//弾
	ModelManager::GetInstance()->LoadModel("bullet", "sphere", "sphere.gltf");
	//カメラ
	ModelManager::GetInstance()->LoadModel("camera", "cube", "cube.obj");
	//地面
	ModelManager::GetInstance()->LoadModel("ground", "cube", "cube.obj");
	//フィールド
	ModelManager::GetInstance()->LoadModel("field", "terrain", "terrain.obj");
	//平面
	ModelManager::GetInstance()->LoadModel("plane", "base", "plane.gltf");
	//HPバー
	ModelManager::GetInstance()->LoadModel("hpBar", "base", "plane.gltf");
	//HPアウトライン
	ModelManager::GetInstance()->LoadModel("hpOutLine", "base", "plane.gltf");
	//スポットライト
	ModelManager::GetInstance()->LoadModel("spotLight", "plane", "plane.obj");
	//軸
	ModelManager::GetInstance()->LoadModel("test", "base", "axis.obj");
}

//カメラの生成
void GameObjectList::CreateCamera() {
	//カメラの管理
	core_->GetCameraManager()->CreateCamera("defaultCamera");
	//デバッグカメラ
	core_->GetCameraManager()->CreateCamera("debugCamera");
	//タイトルカメラ
	core_->GetCameraManager()->CreateCamera("titleCamera");
	//ゲームカメラ
	core_->GetCameraManager()->CreateCamera("gameCamera");
	//リザルトカメラ
	core_->GetCameraManager()->CreateCamera("resultCamera");
}