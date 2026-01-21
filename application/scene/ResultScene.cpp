#include "ResultScene.h"
#include "engine/input/Input.h"
#include "engine/scene/SceneManager.h"

//初期化
void ResultScene::Initialize(DirectXBase* directXBase) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase);
	camera_ = CameraManager::GetInstance()->FindCamera("titleCamera");
}

//更新ww
void ResultScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	} else if (Input::GetInstance()->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();
	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = CameraManager::GetInstance()->FindCamera("titleCamera");
	}
#endif // _DEBUG


}

//描画
void ResultScene::Draw() {

}

//終了
void ResultScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
