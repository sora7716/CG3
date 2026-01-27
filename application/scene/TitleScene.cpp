#include "TitleScene.h"
#include "engine/input/Input.h"
#include "engine/scene/SceneManager.h"
#include "engine/base/StringUtility.h"
#include "engine/2d/TextureManager.h"
#include <cstring>

//初期化
void TitleScene::Initialize(DirectXBase* directXBase) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase);
	camera_ = CameraManager::GetInstance()->FindCamera("titleCamera");

	textRasterizer_ = std::make_unique<TextRasterizer>();
	textRasterizer_->Initialize();
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("monsterBall.png");

	object2d_ = std::make_unique<Object2d>();
	object2d_->Initialize("monsterBall.png");
}

//更新ww
void TitleScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();
	CpuBitmap cpuBitmap = textRasterizer_->RenderTextToCpuBitmap(StringUtility::ConvertString(text_), width, height, StringUtility::ConvertString(fontType_), size, textColor);
	TextureManager::GetInstance()->UpdateTextureFromMemotyBGRA("scoreText", cpuBitmap.bgra.data(), cpuBitmap.width, cpuBitmap.height, cpuBitmap.stride);

	sprite_->SetText("scoreText");
	sprite_->SetTransformData(spriteTransformData_);
	sprite_->Update();

	object2d_->SetText("scoreText");
	object2d_->SetTransformData(object2dTransformData_);
	object2d_->Update();

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
	strncpy_s(buffer1, text_.c_str(), sizeof(buffer1));

	if (ImGui::InputText("inputText", buffer1, sizeof(buffer1))) {
		text_ = buffer1;
	}

	char buffer2[256];
	strncpy_s(buffer2, fontType_.c_str(), sizeof(buffer2));

	if (ImGui::InputText("fontType", buffer2, sizeof(buffer2))) {
		fontType_ = buffer2;
	}
	ImGui::DragInt("width", &width, 1.0f);
	ImGui::DragInt("height", &height, 1.0f);
	ImGui::DragFloat("Size", &size, 1.0f);
	ImGui::DragFloat2("sprite.scale", &spriteTransformData_.scale.x, 1.0f);
	ImGui::DragFloat("sprite.rotate", &spriteTransformData_.rotate, 0.1f);
	ImGui::DragFloat2("sprite.translate", &spriteTransformData_.translate.x, 0.1f);
	ImGui::ColorEdit4("color", &textColor.x);
	/*ImGui::DragFloat2("object2d.scale", &object2dTransformData_.scale.x, 1.0f);
	ImGui::DragFloat("object2d.rotate", &object2dTransformData_.rotate, 0.1f);
	ImGui::DragFloat2("object2d.translate", &object2dTransformData_.translate.x, 0.1f);*/
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
void TitleScene::Draw() {
	//object2d_->Draw();
	sprite_->Draw();
}

//終了
void TitleScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
