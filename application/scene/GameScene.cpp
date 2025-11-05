#include "GameScene.h"
#include "engine/input/Input.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/debug/GlobalVariables.h"

//初期化
void GameScene::Initialize(DirectXBase* directXBase) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase);
	//カメラの設定
	camera_ = CameraManager::GetInstance()->FindCamera("gameCamera");

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(camera_, "player");

	//追従カメラ
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize(camera_);
	cameraController_->SetTarget(player_.get());
	cameraController_->Reset();
	cameraController_->SetMovableArea({ 0.0f,30.0f,30.0f,0.0f});
	//追従カメラ
	cameraController_->Update();

	//フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize(camera_);
}

//更新
void GameScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	//プレイヤー
	player_->Update();

	//追従カメラ
	cameraController_->Update();

	//フィールド
	field_->SetDirectionalLight(Object3dCommon::GetInstance()->GetDirectionalLight());
	field_->SetPointLight(Object3dCommon::GetInstance()->GetPointLight());
	field_->SetSpotLight(Object3dCommon::GetInstance()->GetSpotLight());
	field_->Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();

	//プレイヤー
	ImGui::Begin("player");
	player_->Debug();
	ImGui::End();

	//フィールド
	ImGui::Begin("field");
	field_->Debug();
	ImGui::End();

	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	//追従カメラ
	ImGui::Begin("cameraController");
	cameraController_->Debug();
	ImGui::End();

	//グローバル変数の更新
	GlobalVariables::GetInstance()->Update();

	//Object3dCommon
	Object3dCommon::GetInstance()->Debug();

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
	field_->SetCamera(camera_);
#endif // _DEBUG

}

//描画
void GameScene::Draw() {
	//プレイヤー
	player_->Draw();

	//マップチップ
	field_->Draw();
}

//終了
void GameScene::Finalize() {
	//プレイヤー
	player_->Finalize();

	//フィールド
	field_->Finalize();

	GlobalVariables::GetInstance()->Finalize();

	//シーンのインターフェース
	IScene::Finalize();
}
