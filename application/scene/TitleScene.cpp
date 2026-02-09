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
}

//更新ww
void TitleScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	if (core_->GetInput()->TriggerKey(DIK_SPACE)) {
		core_->GetSceneManager()->ChangeScene("Game");
	} else if (core_->GetInput()->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
		core_->GetSceneManager()->ChangeScene("Game");
	}
#ifdef USE_IMGUI
	//ImGuiの受付開始
	core_->GetImGuiManager()->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	ImGui::Text("Title");

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

}

//終了
void TitleScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
