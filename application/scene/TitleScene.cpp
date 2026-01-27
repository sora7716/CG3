#include "TitleScene.h"
#include "engine/input/Input.h"
#include "engine/scene/SceneManager.h"
#include "engine/math/func/Collision.h"
#include "engine/2d/TextureManager.h"

//初期化
void TitleScene::Initialize(DirectXBase* directXBase) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase);
	camera_ = CameraManager::GetInstance()->FindCamera("titleCamera");

	textRasterizer_ = std::make_unique<TextRasterizer>();
	textRasterizer_->Initialize();
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("monsterBall.png");
	

}

//更新ww
void TitleScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	sprite_->Update();
	CpuBitmap cpuBitmap = textRasterizer_->RenderTextToCpuBitmap(L"Hello", width, height, L"Yu Gothic UI", size);
	TextureManager::GetInstance()->UpdateTextureFromMemotyBGRA("scoreText", cpuBitmap.bgra.data(), cpuBitmap.width, cpuBitmap.height, cpuBitmap.stride);
	sprite_->ChangeTexture("scoreText");

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}else if(Input::GetInstance()->TriggerXboxPad(xBoxPadNumber_,XboxInput::kB)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	ImGui::Text("Title");

	ImGui::DragInt("width", &width,1.0f);
	ImGui::DragInt("height", &height,1.0f);
	ImGui::DragFloat("Size", &size,1.0f);

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
	sprite_->Draw();
}

//終了
void TitleScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
