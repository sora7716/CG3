#include "GameScene.h"
#include "engine/input/Input.h"

//初期化
void GameScene::Initialize(DirectXBase* directXBase) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase);
	//カメラの設定
	camera_ = CameraManager::GetInstance()->FindCamera("gameCamera");

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetCamera(camera_);

	//追従カメラ
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize(camera_);
	cameraController_->SetTarget(player_.get());
	cameraController_->Reset();
	cameraController_->SetMovableArea({ 0.0f,10.0f,0.0f,10.0f });

	//3Dオブジェクト
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize();
	object3d_->SetModel("test");
	object3d_->SetCamera(camera_);
	object3d_->SetTexture("white1x1.png");
}

//更新
void GameScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	//プレイヤー
	player_->Update();

	//追従カメラ
	cameraController_->Update();

	//3Dオブジェクト
	object3d_->Update();

#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();

	//プレイヤー
	ImGui::Begin("Player");
	player_->DebugText();
	ImGui::End();

	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	//追従カメラ
	ImGui::Begin("cameraController");
	cameraController_->Debug();
	ImGui::End();
	
	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	//カメラの切り替え
	if (debugCamera_->IsDebug()) {
		camera_ = CameraManager::GetInstance()->FindCamera("debugCamera");
	} else {
		camera_ = CameraManager::GetInstance()->FindCamera("gameCamera");
	}
	player_->SetCamera(camera_);
	object3d_->SetCamera(camera_);
#endif // _DEBUG

}

//描画
void GameScene::Draw() {
	//プレイヤー
	player_->Draw();

	//3Dオブジェクト
	object3d_->Draw();
}

//終了
void GameScene::Finalize() {
	//プレイヤー
	player_->Finalize();

	//シーンのインターフェース
	IScene::Finalize();
}
