#include "GameScene.h"
#include "engine/input/Input.h"
#include "engine/3d/Object3dCommon.h"

//初期化
void GameScene::Initialize(DirectXBase* directXBase) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase);
	//カメラの設定
	camera_ = CameraManager::GetInstance()->FindCamera("gameCamera");

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(camera_, "player");

	//追従カメラ
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize(camera_);
	cameraController_->SetTarget(player_.get());
	cameraController_->Reset();
	cameraController_->SetMovableArea({ 0.0f,30.0f,0.0f,30.0f,30.0f,0.0f});
	//追従カメラ
	cameraController_->Update();

	//フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize(camera_);

	//obj
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.shininess = 10.0f;
	material_.uvMatrix = Matrix4x4::Identity4x4();
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(camera_);
	object3d_->SetModel("field");
	object3d_->SetTransform(transformData_);
	object3d_->SetCamera(camera_);
	object3d_->GetModel()->SetMaterial(material_);
}

//更新
void GameScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	//プレイヤー
	player_->Update();

	//追従カメラ
	cameraController_->Update();

	//フィールド
	field_->SetDirectionalLight(Object3dCommon::GetInstance()->GetDirectionalLight());
	field_->SetPointLight(Object3dCommon::GetInstance()->GetPointLight());
	field_->SetSpotLight(Object3dCommon::GetInstance()->GetSpotLight());
	field_->Update();

	//obj
	object3d_->GetModel()->SetMaterial(material_);
	object3d_->Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();

	//プレイヤー
	ImGui::Begin("player");
	player_->Debug();
	ImGui::End();

	//フィールド
	ImGui::Begin("field");
	field_->Debug();
	ImGui::End();

	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	//追従カメラ
	ImGui::Begin("cameraController");
	cameraController_->Debug();
	ImGui::End();

	//obj
	ImGui::Begin("obj");
	ImGui::ColorEdit4("color", &material_.color.x);
	ImGui::DragFloat("shiness", &material_.shininess);
	ImGuiManager::CheckBoxToInt("enableLighting", material_.enableLighting);
	ImGui::End();

	//Object3dCommon
	Object3dCommon::GetInstance()->Debug();

	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	//カメラの切り替え
	if (debugCamera_->IsDebug()) {
		camera_ = CameraManager::GetInstance()->FindCamera("debugCamera");
	} else {
		camera_ = CameraManager::GetInstance()->FindCamera("gameCamera");
	}
	player_->SetCamera(camera_);
	field_->SetCamera(camera_);
	object3d_->SetCamera(camera_);
#endif // _DEBUG

}

//描画
void GameScene::Draw() {
	//プレイヤー
	player_->Draw();

	//フィールド
	//ground_->Draw();

	//obj
	//object3d_->Draw();

	//マップチップ
	field_->Draw();
}

//終了
void GameScene::Finalize() {
	//プレイヤー
	player_->Finalize();

	//フィールド
	field_->Finalize();

	//シーンのインターフェース
	IScene::Finalize();
}
