#include "Player.h"
#include "engine/3d/Object3d.h"
#include "engine/camera/Camera.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/input/Input.h"
#include "engine/math/func/Math.h"
#include "engine/debug/GlobalVariables.h"

//初期化
void Player::Initialize(Camera* camera, const std::string& modelName) {
	//入力
	input_ = Input::GetInstance();

	//トランスフォームの初期化
	gameObject_.transfromData = {
		.scale = Vector3::MakeAllOne(),
		.rotate = {},
		.translate = {}
	};
	gameObject_.velocity = {5.0f,0.0f,5.0f};
	gameObject_.acceleration = { 0.1f,0.0f,0.1f };

	//3Dオブジェクトの生成と初期化
	object3d_ = new Object3d();
	object3d_->Initialize(camera);
	object3d_->SetCamera(camera);
	object3d_->SetModel(modelName);

	//マテリアルの初期化
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.shininess = 10.0f;
	material_.uvMatrix = Matrix4x4::Identity4x4();

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	/*globalVariables->SetValue(groupName, "testInt", 90);
	globalVariables->SetValue(groupName, "testFloat", 90.0f);
	globalVariables->SetValue(groupName, "testVector3", { 90.0f,100.0f,20.0f });*/
}

//更新
void Player::Update() {
	//トランスフォームのセット
	object3d_->SetTransform(gameObject_.transfromData);

	//マテリアルのセット
	object3d_->GetModel()->SetMaterial(material_);

	//3Dオブジェクトの更新
	object3d_->Update();
}

//描画
void Player::Draw() {
	//3Dオブジェクトの描画
	object3d_->Draw();

	Move();
}

//デバッグ
void Player::Debug() {
	ImGuiManager::GetInstance()->DragTransform(gameObject_.transfromData);
#ifdef USE_IMGUI
	ImGui::ColorEdit4("color", &material_.color.x);
	ImGui::DragFloat("shininess", &material_.shininess, 0.1f);
	ImGuiManager::CheckBoxToInt("enableLighting", material_.enableLighting);
#endif // USE_IMGUI

}

//終了
void Player::Finalize() {
	//3Dオブジェクトの解放
	delete object3d_;
	object3d_ = nullptr;
}

//カメラのセッター
void Player::SetCamera(Camera*camera) {
	object3d_->SetCamera(camera);
}

//トランスフォームデータのゲッター
TransformData Player::GetTransformData() {
	return gameObject_.transfromData;
}

//速度のゲッター
Vector3 Player::GetVelocity() {
	return gameObject_.velocity;
}

//移動
void Player::Move() {
	//前後
	if (input_->PressKey(DIK_UP)) {
		gameObject_.direction.z = 1.0f;
	} else if (input_->PressKey(DIK_DOWN)) {
		gameObject_.direction.z = -1.0f;
	} else {
		gameObject_.direction.z = 0.0f;
	}

	//左右
	if (input_->PressKey(DIK_RIGHT)) {
		gameObject_.direction.x = 1.0f;
	} else if (input_->PressKey(DIK_LEFT)) {
		gameObject_.direction.x = -1.0f;
	} else {
		gameObject_.direction.x = 0.0f;
	}

	//移動
	//gameObject_.velocity += gameObject_.acceleration;
	gameObject_.transfromData.translate += (gameObject_.velocity * gameObject_.direction.Normalize()) * Math::kDeltaTime;
}
