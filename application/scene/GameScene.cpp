#include "GameScene.h"
#include "engine/input/Input.h"

//初期化
void GameScene::Initialize(DirectXBase* directXBase) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetCamera(debugCamera_->GetCamera());
}

//更新
void GameScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	//プレイヤー
	player_->Update();

#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();

	//プレイヤー
	ImGui::Begin("Player");
	player_->DebugText();
	ImGui::End();
	bool isConect = Input::GetInstance()->IsXboxPadConnected(0);
	Vector2 leftStick = Input::GetInstance()->GetXboxPadLeftStick(0);
	Vector2 rightStick = Input::GetInstance()->GetXboxPadRighttStick(0);
	ImGui::Begin("Xbox");
	ImGui::Checkbox("isConect", &isConect);
	ImGui::DragFloat2("left", &leftStick.x);
	ImGui::DragFloat2("right", &rightStick.x);
	ImGui::End();


	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI
}

//描画
void GameScene::Draw() {
	//プレイヤー
	player_->Draw();
}

//終了
void GameScene::Finalize() {
	//プレイヤー
	player_->Finalize();

	//シーンのインターフェース
	IScene::Finalize();
}
