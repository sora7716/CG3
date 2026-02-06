#include "ResultScene.h"
#include "engine/input/Input.h"
#include "engine/camera/CameraManager.h"
#include "engine/scene/SceneManager.h"

//初期化
void ResultScene::Initialize(DirectXBase* directXBase, Input* input, CameraManager* cameraManager) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase, input, cameraManager);
	camera_ = cameraManager_->FindCamera("ResultCamera");
}

//更新ww
void ResultScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene("Title");
	} else if (input_->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}
#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();
	ImGui::Text("Result");
	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = cameraManager_->FindCamera("ResultCamera");
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
