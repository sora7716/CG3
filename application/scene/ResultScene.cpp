#include "ResultScene.h"
#include "engine/input/Input.h"
#include "engine/camera/CameraManager.h"
#include "engine/scene/SceneManager.h"
#include "engine/debug/ImGuiManager.h"
#include "application/actor/Score.h"
#include "engine/2d/Text.h"

//コンストラクタ
ResultScene::ResultScene() {
}

//デストラクタ
ResultScene::~ResultScene() {
}

//初期化
void ResultScene::Initialize(const SceneContext& sceneContext) {
	//シーンのインタフェースの初期化
	IScene::Initialize(sceneContext);
	camera_ = sceneContext_.cameraManager->FindCamera("ResultCamera");

	//スコア
	score_ = std::make_unique<Score>();
	score_->Initialize(sceneContext_.object2dCommon);

	//PressReturn
	pressReturn_ = std::make_unique<Text>();
	pressReturn_->Initialize(sceneContext_.object2dCommon,"pressReturn");
	pressReturn_->SetText("Press : B");
}

//更新ww
void ResultScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	if (sceneContext_.input->TriggerKey(DIK_SPACE)) {
		sceneContext_.sceneManager->ChangeScene("Title");
	} else if (sceneContext_.input->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
		sceneContext_.sceneManager->ChangeScene("Title");
	}

	score_->SetPosition(scorePos_);
	score_->SetScale(scoreScele_);
	score_->SetTextSize(scoreTextSize_);
	score_->Update();

	pressReturn_->SetTranslate(pressReturnPos_);
	pressReturn_->SetScale(scoreScele_);
	pressReturn_->SetTextSize(pressReturnSize_);
	pressReturn_->Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	sceneContext_.imguiManager->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::SeparatorText("socre");
	ImGui::PushID(0);
	ImGui::DragFloat2("scale", &scoreScele_.x, 0.1f);
	ImGui::DragFloat2("position", &scorePos_.x, 0.1f);
	ImGui::DragFloat("textSize", &scoreTextSize_, 0.1f);
	ImGui::PopID();
	ImGui::SeparatorText("pressReturn");
	ImGui::PushID(1);
	ImGui::DragFloat2("position", &pressReturnPos_.x, 0.1f);
	ImGui::DragFloat("textSize", &pressReturnSize_, 0.1f);
	ImGui::PopID();
	ImGui::End();
	ImGui::Text("Result");
	//ImGuiの受付終了
	sceneContext_.imguiManager->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = sceneContext_.cameraManager->FindCamera("ResultCamera");
	}
#endif // _DEBUG


}

//描画
void ResultScene::Draw() {
	score_->Draw();
	pressReturn_->Draw();
}

//終了
void ResultScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
