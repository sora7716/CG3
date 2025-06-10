#include "TitleScene.h"
#include "engine/input/Input.h"
#include "SceneManager.h"
#include "SceneFactory.h"
#include "engine/3d/ModelManager.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/2d/TextureManager.h"
//初期化
void TitleScene::Initialize(DirectXBase* directXBase) {
	object3d_ = std::make_unique<Object3d>();
	worldTransform3d_.scale = { 1.0f,1.0f,1.0f };
	worldTransform3d_.rotate = {};
	worldTransform3d_.translate = { 0.0f,0.0f,0.0f };
	object3d_->Initialize();
	object3d_->SetModel("cube");
	//object3d_->SetTexture("uvChecker");

	directionalLight_.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight_.intensity = 1.0f;

	particleSystem_ = std::make_unique<ParticleSystem>();
	//TextureManager::GetInstance()->LoadTexture("engine/resources/textures/monsterBall.png");
	particleSystem_->Initialize(directXBase);

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("engine/resources/textures/sample.png");
}

//更新
void TitleScene::Update() {
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();

	object3d_->SetScale(worldTransform3d_.scale);
	object3d_->SetRotate(worldTransform3d_.rotate);
	object3d_->SetTranslate(worldTransform3d_.translate);
	object3d_->SetColor(object3dColor_);
	Object3dCommon::GetInstance()->SetDirectionalLightData(directionalLight_);
	object3d_->Update();

	particleSystem_->Update();

	sprite_->Update();
#ifdef USE_IMGUI
	ImGui::Begin("3dModel");
	ImGui::DragFloat3("scale", &worldTransform3d_.scale.x, 0.1f);
	ImGui::DragFloat3("rotate", &worldTransform3d_.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &worldTransform3d_.translate.x, 0.1f);
	ImGui::DragFloat4("color", &object3dColor_.x, 0.1f);
	ImGui::ColorEdit4("color", &object3dColor_.x);
	ImGui::End();

	ImGui::Begin("suji");
	ImGui::End();

	ImGui::Begin("light");
	ImGui::ColorEdit4("color", &directionalLight_.color.x);
	ImGui::DragFloat3("direction", &directionalLight_.direction.x, 0.1f);
	ImGui::DragFloat("intensity", &directionalLight_.intensity, 0.1f, 0.0f, 10.0f);
	ImGui::End();

	ImGui::Begin("blend");
	/*const char* blendModes3d[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };
	if (ImGui::Combo("3dMode", &blendMode_, blendModes3d, IM_ARRAYSIZE(blendModes3d))) {
		object3d_->SetBlendMode((BlendMode)blendMode_);
	}*/
	const char* blendModes2d[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };
	if (ImGui::Combo("3dMode", &blendMode_, blendModes2d, IM_ARRAYSIZE(blendModes2d))) {
		object3d_->SetBlendMode((BlendMode)blendMode_);
	}
	ImGui::End();

	/*ImGui::Begin("camera");
	ImGui::DragFloat3("cameraPos", &cameraRotate_.x, 0.1f);
	ImGui::End();
	CameraManager::GetInstance()->FindCamera("defaultCamera")->SetRotate(cameraRotate_);*/
#endif // USE_IMGUI
	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
}

//描画
void TitleScene::Draw() {
	particleSystem_->Draw();
	object3d_->Draw();
	sprite_->Draw();
}

//終了
void TitleScene::Finalize() {
	particleSystem_->Finalize();
}
