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

	//追従カメラ
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize(camera_, {}, {15.5f,11.7f,0.0f});

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(railCamera_->GetCamera(), "player");
	player_->SetPosition({ 0.0f,0.0f,10.0f });

	//フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize(railCamera_->GetCamera());

#ifdef _DEBUG
	//制御ポイント
	controlPoint_ = std::make_unique<ControlPoint>();
	controlPoint_->Initialize(camera_);
#endif // _DEBUG
}

//更新
void GameScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	//プレイヤー
	player_->GetObject3d()->SetParent(railCamera_->GetWorldTransform());
	player_->Update();

	//制御ポイント
	controlPoint_->Update();

	//追従カメラ
	railCamera_->SetControlPoints(controlPoint_->GetControlPoints());
	railCamera_->Update();

	//フィールド
	field_->SetDirectionalLight(Object3dCommon::GetInstance()->GetDirectionalLight());
	field_->SetPointLight(Object3dCommon::GetInstance()->GetPointLight());
	field_->SetSpotLight(Object3dCommon::GetInstance()->GetSpotLightPtr());
	field_->Update();

#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	//グローバル変数の更新
	GlobalVariables::GetInstance()->Update();

	//プレイヤー
	ImGui::Begin("player");
	player_->Debug();
	ImGui::End();

	//Object3dCommon
	Object3dCommon::GetInstance()->Debug();

	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	//カメラの切り替え
	if (debugCamera_->IsDebug()) {
		player_->SetCamera(debugCamera_->GetCamera());
		field_->SetCamera(debugCamera_->GetCamera());
		railCamera_->SetCamera(debugCamera_->GetCamera());
		controlPoint_->SetCamera(debugCamera_->GetCamera());
	} else {
		player_->SetCamera(railCamera_->GetCamera());
		field_->SetCamera(railCamera_->GetCamera());
		railCamera_->SetCamera(railCamera_->GetCamera());
		controlPoint_->SetCamera(railCamera_->GetCamera());
	}
#endif // _DEBUG

}

//描画
void GameScene::Draw() {
#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		//レイルカメラ
		railCamera_->Draw();
	}
#endif // _DEBUG

	//プレイヤー
	player_->Draw();

	//マップチップ
	field_->Draw();

#ifdef _DEBUG
	//制御ポイント
	controlPoint_->Draw();
#endif // _DEBUG
}

//終了
void GameScene::Finalize() {
	//プレイヤー
	player_->Finalize();

	//フィールド
	field_->Finalize();

	//レールカメラ
	railCamera_->Finalize();

#ifdef _DEBUG
	//制御ポイント
	controlPoint_->Finalize();
#endif // _DEBUG


	//グローバル変数
	GlobalVariables::GetInstance()->Finalize();

	//シーンのインターフェース
	IScene::Finalize();
}
