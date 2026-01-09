#include "Player.h"
#include "engine/3d/Object3d.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/camera/Camera.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/input/Input.h"
#include "engine/math/func/Math.h"
#include "engine/worldTransform/WorldTransform.h"
#include "Bullet.h"

//初期化
void Player::Initialize(Camera* camera, const std::string& modelName) {
	//入力
	input_ = Input::GetInstance();

	//カメラ
	camera_ = camera;

	//トランスフォームの初期化
	playerData_.gameObject.transformData = {
		.scale = Vector3::MakeAllOne(),
		.rotate = {},
		.translate = {}
	};
	playerData_.gameObject.velocity = { 5.0f,5.0f,0.0f };

	//3Dオブジェクトの生成と初期化
	playerData_.object3d = new Object3d();
	playerData_.object3d->Initialize(camera_);
	playerData_.object3d->SetModel(modelName);

	//回転中心の生成と初期化
	//rotateCenter_ = new Object3d();
	//rotateCenter_->Initialize(camera_);
	//rotateCenter_->SetModel(modelName);

	//マテリアルの初期化
	playerData_.gameObject.material.color = { 1.0f,1.0f,1.0f,1.0f };
	playerData_.gameObject.material.enableLighting = true;
	playerData_.gameObject.material.shininess = 10.0f;
	playerData_.gameObject.material.uvMatrix = Matrix4x4::Identity4x4();

	//リムライトの初期化
	rimLight_.color = { 1.0f,1.0f,1.0f,1.0f };
	rimLight_.power = 3.4f;
	rimLight_.outLinePower = 0.3f;
	rimLight_.softness = 0.2f;
	rimLight_.enableRimLighting = true;

	//弾
	bullet_ = new Bullet();
	bullet_->Initialize(camera_);
	bullet_->SetAliveRange(200.0f);
	bullet_->SetSpeed(50.0f);
	bullet_->SetSize({ 0.5f,0.5f,0.5f });
	bullet_->SetMaxBulletCount(10);

	//スポットライトを設定
	Object3dCommon::GetInstance()->AddSpotLight("headlight");
	headlight_ = Object3dCommon::GetInstance()->GetSpotLight("headlight");
}

//更新
void Player::Update() {
	//移動
	Move();

	//攻撃
	Attack();

	//マテリアルのセット
	playerData_.object3d->GetModel()->SetMaterial(playerData_.gameObject.material);

	//リムライトのセット
	playerData_.object3d->GetModel()->SetRimLight(rimLight_);

	//ライトの位置をプレイヤーの前に設定
	headlight_.position = playerData_.object3d->GetWorldPos();
	// ローカル空間での“前”の向き
	Vector3 localForward = { 0.0f, 0.0f, 1.0f };

	//プレイヤーのワールド行列を取得
	Matrix4x4 world = playerData_.object3d->GetWorldTransform()->GetWorldMatrix();

	//前方向ベクトルだけをワールド空間に変換
	Vector3 worldForward = Math::TransformNormal(localForward, world);

	//正規化してライトの方向に設定
	headlight_.direction = worldForward.Normalize();
	//ライトのセッター
	Object3dCommon::GetInstance()->SetSpotLight("headlight", headlight_);

	//回転中心の更新
	//rotateCenter_->SetTransform(rotateCenterTransformData_);
	//rotateCenter_->Update();

	//3Dオブジェクトの更新
	//playerData_.object3d->SetParent(rotateCenter_->GetWorldTransform());
	playerData_.object3d->Update();
}

//描画
void Player::Draw() {
	//3Dオブジェクトの描画
	playerData_.object3d->Draw();
	//弾の描画
	bullet_->Draw();
}

//デバッグ用
void Player::Debug() {
#ifdef USE_IMGUI
	ImGui::DragFloat3("rotate", &playerData_.gameObject.transformData.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &playerData_.gameObject.transformData.translate.x, 0.1f);
	ImGui::ColorEdit4("rimColor", &rimLight_.color.x);
	ImGui::DragFloat("rimPower", &rimLight_.power, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("rimSoftness", &rimLight_.softness, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("rimOutLinePower", &rimLight_.outLinePower, 0.1f, 0.0f, 10.0f);
	ImGuiManager::GetInstance()->CheckBoxToInt("enableRimLighting", rimLight_.enableRimLighting);
#endif // USE_IMGUI
}

//終了
void Player::Finalize() {
	//3Dオブジェクトの解放
	delete playerData_.object3d;
	playerData_.object3d = nullptr;
	//弾の解放
	bullet_->Finalize();
	delete bullet_;
	bullet_ = nullptr;
}

//カメラのセッター
void Player::SetCamera(Camera* camera) {
	playerData_.object3d->SetCamera(camera);
}

//位置のセッター
void Player::SetPosition(const Vector3& positBulletion) {
	playerData_.gameObject.transformData.translate = positBulletion;
}

//オブジェクト3dのゲッター
Object3d* Player::GetObject3d() {
	return playerData_.object3d;
}

//トランスフォームデータのゲッター
TransformData Player::GetTransformData() {
	return playerData_.gameObject.transformData;
}

//速度のゲッター
Vector3 Player::GetVelocity() {
	return playerData_.gameObject.velocity;
}

//移動
void Player::Move() {
	//移動しているかのフラグのリセット
	playerData_.isMove = false;

	//前後
	if (input_->PressKey(DIK_UP)) {
		playerData_.gameObject.direction.y = 1.0f;
		playerData_.isMove = true;
	} else if (input_->PressKey(DIK_DOWN)) {
		playerData_.gameObject.direction.y = -1.0f;
		playerData_.isMove = true;
	} else {
		playerData_.gameObject.direction.y = 0.0f;
	}

	//左右
	if (input_->PressKey(DIK_RIGHT)) {
		playerData_.gameObject.direction.x = 1.0f;
		playerData_.isMove = true;
	} else if (input_->PressKey(DIK_LEFT)) {
		playerData_.gameObject.direction.x = -1.0f;
		playerData_.isMove = true;
	} else {
		playerData_.gameObject.direction.x = 0.0f;
	}

	//移動
	playerData_.gameObject.transformData.translate += (playerData_.gameObject.velocity * playerData_.gameObject.direction.Normalize()) * Math::kDeltaTime;

	//トランスフォームのセット
	playerData_.object3d->SetTransform(playerData_.gameObject.transformData);
}

//攻撃
void Player::Attack() {
	//弾の発射
	bullet_->SetShootingPosition(playerData_.object3d->GetWorldPos());
	bullet_->SetSourceWorldMatrix(playerData_.object3d->GetWorldTransform()->GetWorldMatrix());
	bullet_->Fire(input_->TriggerKey(DIK_SPACE));
	bullet_->Update();
}