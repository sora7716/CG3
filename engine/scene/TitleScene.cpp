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
	//デバックカメラ
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();

	//3Dモデル
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize();
	object3d_->SetModel("cube");
	transformData3d_.translate = { 0.0f,0.0f,10.0f };
	object3d_->SetCamera(debugCamera_->GetCamera());


	//パーティクル
	ParticleManager::GetInstance()->AddParticleSystem("par");
	ParticleManager::GetInstance()->FindParticleSystem("par")->Initialize(directXBase, "monsterBall.png", ModelManager::GetInstance()->FindModel("sphere"));
	ParticleManager::GetInstance()->FindParticleSystem("par")->SetCamera(debugCamera_->GetCamera());
}

//更新ww
void TitleScene::Update() {
	//デバックカメラ
	debugCamera_->Update();

	//3Dモデル
	object3d_->SetTransform(transformData3d_);
	object3d_->Update();

	//パーティクル
	ParticleManager::GetInstance()->FindParticleSystem("par")->Update();

#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();

	ImGui::Begin("object3d");
	ImGui::DragFloat3("rotate", &transformData3d_.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &transformData3d_.translate.x, 0.1f);
	ImGui::End();

	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	ImGui::Begin("particle");
	ParticleManager::GetInstance()->FindParticleSystem("par")->Debug();
	ImGui::End();
	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI

}

//描画
void TitleScene::Draw() {
	//3Dモデル
	object3d_->Draw();

	//パーティクル
	ParticleManager::GetInstance()->FindParticleSystem("par")->Draw();
}

//終了
void TitleScene::Finalize() {

}
