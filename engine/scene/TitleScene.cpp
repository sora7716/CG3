#include "TitleScene.h"
#include "engine/input/Input.h"
#include "SceneManager.h"
#include "SceneFactory.h"
#include "engine/3d/ModelManager.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/2d/TextureManager.h"
#include "engine/math/func/Math.h"
#include "engine/particle/ParticleManager.h"

//初期化
void TitleScene::Initialize(DirectXBase* directXBase) {
	for (int32_t i = 0; i < 2; i++) {
		object3des_[i] = std::make_unique<Object3d>();
		worldTransform3d_[i].rotate = {};
		worldTransform3d_[i].scale = { 1.0f,1.0f,1.0f };
		object3des_[i]->SetModel("sphere");
		object3des_[i]->SetTexture("uvChecker.png");
		object3des_[i]->Initialize();
	}
	object3des_[1]->SetParent(object3des_[0]->GetWorldTransform());
	worldTransform3d_[0].translate = { 0.0f,0.0f,0.0f };
	worldTransform3d_[1].translate = { 2.0f,2.0f,0.0f };

	directionalLight_.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight_.intensity = 1.0f;

	ParticleManager::GetInstance()->AddParticleSystem("d");
	ParticleManager::GetInstance()->FindParticleSystem("d")->Initialize(directXBase, "circle.png");

}

//更新
void TitleScene::Update() {
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();

	Object3dCommon::GetInstance()->SetDirectionalLightData(directionalLight_);


	for (int32_t i = 0; i < 2; i++) {
		object3des_[i]->SetScale(worldTransform3d_[i].scale);
		object3des_[i]->SetRotate(worldTransform3d_[i].rotate);
		object3des_[i]->SetTranslate(worldTransform3d_[i].translate);
		object3des_[i]->SetColor(object3dColor_);
		object3des_[i]->Update();
	}

	ParticleManager::GetInstance()->FindParticleSystem("d")->Update();
#ifdef USE_IMGUI
	ImGui::Begin("3dModel");
	ImGui::DragFloat3("scale[0]", &worldTransform3d_[0].scale.x, 0.1f);
	ImGui::DragFloat3("rotate[0]", &worldTransform3d_[0].rotate.x, 0.1f);
	ImGui::DragFloat3("translate[0]", &worldTransform3d_[0].translate.x, 0.1f);
	ImGui::DragFloat3("scale[1]", &worldTransform3d_[1].scale.x, 0.1f);
	ImGui::DragFloat3("rotate[1]", &worldTransform3d_[1].rotate.x, 0.1f);
	ImGui::DragFloat3("translate[1]", &worldTransform3d_[1].translate.x, 0.1f);
	ImGui::DragFloat4("color", &object3dColor_.x, 0.1f);
	ImGui::ColorEdit4("color", &object3dColor_.x);
	ImGui::End();

	ImGui::Begin("light");
	ImGui::ColorEdit4("color", &directionalLight_.color.x);
	ImGui::DragFloat3("direction", &directionalLight_.direction.x, 0.1f);
	ImGui::DragFloat("intensity", &directionalLight_.intensity, 0.1f, 0.0f, 10.0f);
	ImGui::End();

	//ImGui::Begin("blend");
	///*const char* blendModes3d[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };
	//if (ImGui::Combo("3dMode", &blendMode_, blendModes3d, IM_ARRAYSIZE(blendModes3d))) {
	//	object3d_->SetBlendMode((BlendMode)blendMode_);
	//}*/
	//const char* blendModes2d[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };
	//if (ImGui::Combo("3dMode", &blendMode_, blendModes2d, IM_ARRAYSIZE(blendModes2d))) {
	//	object3des_->SetBlendMode((BlendMode)blendMode_);
	//}
	//ImGui::End();

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
	for (auto& object3d : object3des_) {
		object3d->Draw();
	}
	ParticleManager::GetInstance()->FindParticleSystem("d")->Draw();
}

//終了
void TitleScene::Finalize() {

}
