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
	//エミッター
	emitter_ = {
		.transform = {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}},
		.count = 1,
		.frequency = 0.1f,//発生頻度
		.frequencyTime = 0.0f,//発生頻度用の時刻,0.0fで初期化
		.range = 1.0f
	};


	//加速度フィールド
	accelerationField_.acceleration = {};
	accelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField_.area.max = { 1.0f,1.0f,1.0f };
	ParticleManager::GetInstance()->AddParticleSystem("testParticle");
	particleSystem_ = ParticleManager::GetInstance()->FindParticleSystem("testParticle");
	particleSystem_->Initialize(directXBase, "uvChecker.png", ModelManager::GetInstance()->FindModel("sphere"));
	particleSystem_->SetCamera(debugCamera_->GetCamera());
}

//更新ww
void TitleScene::Update() {
	//デバックカメラ
	debugCamera_->Update();

	//3Dモデル
	object3d_->SetTransform(transformData3d_);
	object3d_->Update();

	//パーティクル
	particleSystem_->SetTransformData(emitter_.transform);
	particleSystem_->SetParticleCount(emitter_.count);
	particleSystem_->SetEmitRange(emitter_.range);
	particleSystem_->SetAccelerationField(accelerationField_);
	particleSystem_->Update();

#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();

	ImGui::Begin("object3d");
	ImGui::DragFloat3("scale", &transformData3d_.scale.x, 0.1f);
	ImGui::DragFloat3("rotate", &transformData3d_.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &transformData3d_.translate.x, 0.1f);
	ImGui::End();

	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	ImGui::Begin("particle");
	particleSystem_->Debug();
	ImGui::DragFloat3("acceleration", &accelerationField_.acceleration.x, 0.1f);
	ImGui::DragFloat3("scale", &emitter_.transform.scale.x, 0.1f);
	ImGui::DragFloat3("rotate", &emitter_.transform.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &emitter_.transform.translate.x, 0.1f);
	ImGui::DragFloat("emitRange", &emitter_.range, 0.1f, 0.0f, 10.0f);
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
	particleSystem_->Draw();
}

//終了
void TitleScene::Finalize() {

}
