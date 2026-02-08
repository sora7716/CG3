#include "ResultScene.h"
#include "engine/input/Input.h"
#include "engine/camera/CameraManager.h"
#include "engine/scene/SceneManager.h"
#include "engine/base/Core.h"

//初期化
void ResultScene::Initialize(Core* core) {
	//シーンのインタフェースの初期化
	IScene::Initialize(core);
	camera_ = core_->GetCameraManager()->FindCamera("ResultCamera");
}

//更新ww
void ResultScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	if (core_->GetInput()->TriggerKey(DIK_SPACE)) {
		core_->GetSceneManager()->ChangeScene("Title");
	} else if (core_->GetInput()->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
		core_->GetSceneManager()->ChangeScene("Title");
	}
#ifdef USE_IMGUI
	//ImGuiの受付開始
	core_->GetImGuiManager()->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();
	ImGui::Text("Result");
	//ImGuiの受付終了
	core_->GetImGuiManager()->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = core_->GetCameraManager()->FindCamera("ResultCamera");
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
