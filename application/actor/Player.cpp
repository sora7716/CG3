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
	gameObject_.transformData = {
		.scale = Vector3::MakeAllOne(),
		.rotate = {},
		.translate = {}
	};
	gameObject_.velocity = { 5.0f,0.0f,5.0f };

	//3Dオブジェクトの生成と初期化
	gameObject_.object3d = new Object3d();
	gameObject_.object3d->Initialize(camera_,TransformMode::kDirectionToDirection);
	gameObject_.object3d->SetModel(modelName);

	//マテリアルの初期化
	gameObject_.material.color = { 1.0f,1.0f,1.0f,1.0f };
	gameObject_.material.enableLighting = true;
	gameObject_.material.shininess = 10.0f;
	gameObject_.material.uvMatrix = Matrix4x4::Identity4x4();

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
	gameObject_.object3d->GetModel()->SetMaterial(gameObject_.material);

	//リムライトのセット
	gameObject_.object3d->GetModel()->SetRimLight(rimLight_);

	//ヘッドライトの更新
	HeadlightUpdate();

	//マウスの位置
	Vector3 mousePos = input_->GetWorldMousePosition(camera_);
	Vector3 dir = mousePos - gameObject_.object3d->GetWorldPos();
	gameObject_.object3d->GetWorldTransform()->SetFromAndToPos(gameObject_.object3d->GetWorldPos(),dir);
	//3Dオブジェクトの更新
	gameObject_.object3d->Update();
}

//描画
void Player::Draw() {
	//3Dオブジェクトの描画
	gameObject_.object3d->Draw();
	//弾の描画
	bullet_->Draw();
}

//デバッグ用
void Player::Debug() {
#ifdef USE_IMGUI
	ImGui::DragFloat3("rotate", &gameObject_.transformData.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &gameObject_.transformData.translate.x, 0.1f);
	ImGui::ColorEdit4("rimColor", &rimLight_.color.x);
	ImGui::DragFloat("rimPower", &rimLight_.power, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("rimSoftness", &rimLight_.softness, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("rimOutLinePower", &rimLight_.outLinePower, 0.1f, 0.0f, 100.0f);
	ImGuiManager::GetInstance()->CheckBoxToInt("enableRimLighting", rimLight_.enableRimLighting);
#endif // USE_IMGUI
}

//終了
void Player::Finalize() {
	//3Dオブジェクトの解放
	delete gameObject_.object3d;
	gameObject_.object3d = nullptr;
	//弾の解放
	bullet_->Finalize();
	delete bullet_;
	bullet_ = nullptr;
}

//カメラのセッター
void Player::SetCamera(Camera* camera) {
	gameObject_.object3d->SetCamera(camera);
}

//位置のセッター
void Player::SetPosition(const Vector3& positBulletion) {
	gameObject_.transformData.translate = positBulletion;
}

//オブジェクト3dのゲッター
Object3d* Player::GetObject3d() {
	return gameObject_.object3d;
}

//トランスフォームデータのゲッター
TransformData Player::GetTransformData() {
	return gameObject_.transformData;
}

//速度のゲッター
Vector3 Player::GetVelocity() {
	return gameObject_.velocity;
}

//移動
void Player::Move() {
	//前後
	if (input_->PressKey(DIK_UP)) {
		gameObject_.moveDirection.z = 1.0f;
	} else if (input_->PressKey(DIK_DOWN)) {
		gameObject_.moveDirection.z = -1.0f;
	} else {
		gameObject_.moveDirection.z = 0.0f;
	}

	//左右
	if (input_->PressKey(DIK_RIGHT)) {
		gameObject_.moveDirection.x = 1.0f;
	} else if (input_->PressKey(DIK_LEFT)) {
		gameObject_.moveDirection.x = -1.0f;
	} else {
		gameObject_.moveDirection.x = 0.0f;
	}

	//移動
	gameObject_.transformData.translate += (gameObject_.velocity * gameObject_.moveDirection.Normalize()) * Math::kDeltaTime;

	//トランスフォームのセット
	gameObject_.object3d->SetTransform(gameObject_.transformData);
}

//攻撃
void Player::Attack() {
	//弾の発射
	bullet_->SetShootingPosition(gameObject_.object3d->GetWorldPos());
	bullet_->SetSourceWorldMatrix(gameObject_.object3d->GetWorldTransform()->GetWorldMatrix());
	bullet_->Fire(input_->TriggerKey(DIK_SPACE));
	bullet_->Update();
}

//ヘッドライトの更新
void Player::HeadlightUpdate() {
	//ライトの位置をプレイヤーの前に設定
	Vector3 headlightOffset = { 0.0f, 0.0f, 1.0f };
	headlight_.position = gameObject_.object3d->GetWorldPos() + headlightOffset;
	// ローカル空間での“前”の向き
	Vector3 localForward = { 0.0f, 0.0f, 1.0f };

	//プレイヤーのワールド行列を取得
	Matrix4x4 world = gameObject_.object3d->GetWorldTransform()->GetWorldMatrix();

	//前方向ベクトルだけをワールド空間に変換
	Vector3 worldForward = Math::TransformNormal(localForward, world);

	//正規化してライトの方向に設定
	headlight_.direction = worldForward.Normalize();
	headlight_.intensity = 30.0f;
	//ライトのセッター
	Object3dCommon::GetInstance()->SetSpotLight("headlight", headlight_);
}
