#include "TitleScene.h"
#include "engine/input/Input.h"
#include "engine/scene/SceneManager.h"
#include "engine/base/StringUtility.h"
#include "engine/2d/TextureManager.h"
#include "engine/camera/CameraManager.h"
#include <cstring>

//初期化
void TitleScene::Initialize(DirectXBase* directXBase, Input* input, CameraManager* cameraManager) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase, input, cameraManager);
	camera_ = cameraManager_->FindCamera("titleCamera");

	textObj_ = std::make_unique<Text>();
	textObj_->Initialize("testText");
	textStyele_.text = "飯塚 大空";
	textStyele_.font = "Impact";
	textStyele_.size = 128.0f;
	textStyele_.color = Vector4::MakeWhiteColor();
	textTransformData_.scale = { 200.0f,200.0f };
	textTransformData_.rotate = 0.0f;
	textTransformData_.translate = {};
}

//更新ww
void TitleScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	textObj_->SetTextStyle(textStyele_);
	textObj_->SetTransformDate(textTransformData_);
	textObj_->Update();

	//if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
	//	SceneManager::GetInstance()->ChangeScene("Game");
	//} else if (Input::GetInstance()->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
	//	SceneManager::GetInstance()->ChangeScene("Game");
	//}
#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	ImGui::Text("Title");

	ImGui::Begin("text");
	char buffer1[256];
	strncpy_s(buffer1, textStyele_.text.c_str(), sizeof(buffer1));

	if (ImGui::InputText("text", buffer1, sizeof(buffer1))) {
		textStyele_.text = buffer1;
	}

	char buffer2[256];
	strncpy_s(buffer2, textStyele_.font.c_str(), sizeof(buffer2));

	if (ImGui::InputText("font", buffer2, sizeof(buffer2))) {
		textStyele_.font = buffer2;
	}
	ImGui::DragFloat2("scale", &textTransformData_.scale.x, 0.1f);
	ImGui::DragFloat("rotate", &textTransformData_.rotate, 0.1f);
	ImGui::DragFloat2("translate", &textTransformData_.translate.x, 0.1f);
	ImGui::DragFloat("textSize", &textStyele_.size, 0.1f);
	ImGui::ColorEdit4("color", &textStyele_.color.x);
	ImGui::End();

	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = cameraManager_->FindCamera("titleCamera");
	}
#endif // _DEBUG


}

//描画
void TitleScene::Draw() {
	textObj_->Draw();
}

//終了
void TitleScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
