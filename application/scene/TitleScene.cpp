#include "TitleScene.h"
#include "engine/input/Input.h"
#include "engine/scene/SceneManager.h"
#include "engine/base/StringUtility.h"
#include "engine/2d/TextureManager.h"
#include "engine/camera/CameraManager.h"
#include "engine/base/Core.h"
#include <cstring>

//初期化
void TitleScene::Initialize(Core* core) {
	//シーンのインタフェースの初期化
	IScene::Initialize(core);
	camera_ = core_->GetCameraManager()->FindCamera("titleCamera");

	score_ = std::make_unique<Score>();
	score_->Initialize(core_->GetObject2dCommon());
}

//更新ww
void TitleScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	if (core_->GetInput()->PressKey(DIK_SPACE)) {
		score_->AddScore(1);
	}
	//if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
	//	SceneManager::GetInstance()->ChangeScene("Game");
	//} else if (Input::GetInstance()->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
	//	SceneManager::GetInstance()->ChangeScene("Game");
	//}
	score_->Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	core_->GetImGuiManager()->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	ImGui::Text("Title");

	ImGui::Begin("Text");
	score_->Debug();
	ImGui::End();

	//ImGuiの受付終了
	core_->GetImGuiManager()->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = core_->GetCameraManager()->FindCamera("titleCamera");
	}
#endif // _DEBUG


}

//描画
void TitleScene::Draw() {
	score_->Draw();
}

//終了
void TitleScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
