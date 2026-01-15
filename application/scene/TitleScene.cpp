#include "TitleScene.h"
#include "engine/input/Input.h"
#include "engine/scene/SceneManager.h"
#include "engine/scene/SceneFactory.h"
#include "engine/3d/ModelManager.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/2d/TextureManager.h"
#include "engine/math/func/Math.h"
#include "engine/particle/ParticleManager.h"

//初期化
void TitleScene::Initialize(DirectXBase* directXBase) {
	//シーンのインタフェースの初期化
	IScene::Initialize(directXBase);

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(Object3dCommon::GetInstance()->GetDefaultCamera());
	object3d_->SetModel("plane");
	transform_.scale = { 1.0f,1.0f,1.0f };
	material_.shininess = 10.0f;
	material_.color = Vector4::MakeWhiteColor();
	material_.enableLighting = true;
}

//更新ww
void TitleScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	object3d_->SetTransform(transform_);
	object3d_->GetModel()->SetMaterial(material_);
	object3d_->Update();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
#ifdef USE_IMGUI
	//ImGuiの受付開始
	ImGuiManager::GetInstance()->Begin();
	ImGui::ColorEdit4("color", &material_.color.x);
	ImGui::DragFloat("shininess", &material_.shininess,0.1f);
	ImGuiManager::CheckBoxToInt("enableLighting", material_.enableLighting);
	ImGuiManager::DragTransform(transform_);
	Object3dCommon::GetInstance()->Debug();
	//ImGuiの受付終了
	ImGuiManager::GetInstance()->End();
#endif // USE_IMGUI

}

//描画
void TitleScene::Draw() {
	object3d_->Draw();
}

//終了
void TitleScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
