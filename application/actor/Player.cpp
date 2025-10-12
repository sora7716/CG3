#include "Player.h"
#include "engine/3d/Object3d.h"
#include "engine/camera/Camera.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/input/Input.h"
#include "engine/math/func/Math.h"

//初期化
void Player::Initialize() {
	//入力
	input_ = Input::GetInstance();
	//トランスフォームの初期化
	gameObject_.transfromData = {
		.scale = Vector3::MakeAllOne(),
		.rotate = {},
		.translate = {}
	};
	gameObject_.velocity = Vector3::MakeAllOne();
	//3Dオブジェクトの生成と初期化
	object3d_ = new Object3d();
	object3d_->Initialize();
	object3d_->SetModel("cube");
}

//更新
void Player::Update() {
	//3Dオブジェクトの更新
	object3d_->SetTransform(gameObject_.transfromData);
	object3d_->Update();
}

//描画
void Player::Draw() {
	//3Dオブジェクトの描画
	object3d_->Draw();

	Move();
}

//デバッグテキスト
void Player::DebugText() {
	ImGuiManager::GetInstance()->DragTransfrom(gameObject_.transfromData);
}

//終了
void Player::Finalize() {
	//3Dオブジェクトの解放
	delete object3d_;
	object3d_ = nullptr;
}

//カメラのセッター
void Player::SetCamera(Camera* camera) {
	object3d_->SetCamera(camera);
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
	gameObject_.transfromData.translate += (gameObject_.velocity * gameObject_.direction.Normalize()) * Math::kDeltaTime;
}
