#define NOMINMAX
#include "Player.h"
#include "engine/3d/Object3d.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/camera/Camera.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/input/Input.h"
#include "engine/math/func/Math.h"
#include "engine/math/func/Physics.h"
#include "engine/worldTransform/WorldTransform.h"
#include "Bullet.h"
#include "engine/debug/WireframeObject3d.h"
#include "engine/2d/Sprite.h"

//デストラクタ
Player::~Player() {
	Finalize();
}

//初期化
void Player::Initialize(Input* input, SpriteCommon* spriteCommon, Object3dCommon* object3dCommon, Camera* camera, const std::string& modelName) {
	//入力
	input_ = input;

	//3dオブジェクトの共通部分
	object3dCommon_ = object3dCommon;

	//カメラ
	camera_ = camera;

	//トランスフォームの初期化
	gameObject_.transformData = {
		.scale = Vector3::MakeAllOne(),
		.rotate = {},
		.translate = {}
	};
	gameObject_.velocity = { 5.0f,0.0f,5.0f };
	gameObject_.acceleration = { 0.0f,Physics::kGravity,0.0f };

	//3Dオブジェクトの生成と初期化
	gameObject_.object3d = new Object3d();
	gameObject_.object3d->Initialize(object3dCommon, camera_);
	gameObject_.object3d->SetModel(modelName);
	gameObject_.isAlive = true;

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
	bullet_->Initialize(object3dCommon, camera_);
	bullet_->SetAliveRange(kBulletAliveAreaSize);
	bullet_->SetSpeed(kBulletSpeed);
	bullet_->SetSize({ kBulletSize,kBulletSize,kBulletSize });
	bullet_->SetMaxBulletCount(kBulletCount);

	//スポットライトを設定
	headlight_ = object3dCommon_->GetSpotLightPtr()[0];
	headlight_.cosAngle = 0.9f;
	headlight_.cosFolloffStart = 1.2f;
	headlight_.enableSpotLighting = true;

	//ヒットボックス
	gameObject_.hitBox = new WireframeObject3d();
	gameObject_.hitBox->Initialize(object3dCommon_->GetWireframeObject3dCommon(), camera_, ModelType::kCube);
	hitBoxScale_ = Vector3::MakeAllOne() - 0.2f;

	//HPバー
	//中身
	hpBar_ = new Sprite();
	hpBar_->Initialize(spriteCommon, "playerHpBar.png");
	hpBar_->SetBlendMode(BlendMode::kNormal);
	hpBarTransform_.scale = { hpBarWidth_,50.0f };
	hpBarTransform_.translate = { 440.0f,630.0f };
	hpColor_ = Vector4::ColorCodeTransform("#138A0DFF");
	//アウトライン
	hpOutLine_ = new Sprite();
	hpOutLine_->Initialize(spriteCommon, "playerHpOutLine.png");
	hpOutLine_->SetBlendMode(BlendMode::kNormal);
	hpOutLineTransform_.scale = { hpBarWidth_,50.0f };
	hpOutLineTransform_.translate = { 440.0f,630.0f };

}

//更新
void Player::Update() {
	//死亡判定
	Dead();

	//ダメージクールタイムを減らす
	if (damageCoolTime_ > 0.0f) {
		damageCoolTime_ -= Math::kDeltaTime;
	}

	//移動
	Move();

	//ジャンプ
	Jump();

	//速度と加速度を位置に適応
	IntegrateMotion();

	//移動の制限
	gameObject_.transformData.translate.x = std::clamp(gameObject_.transformData.translate.x, 11.0f, 26.0f);
	gameObject_.transformData.translate.z = std::clamp(gameObject_.transformData.translate.z, 3.0f, 29.0f);
	//下に行き過ぎないように制限
	gameObject_.transformData.translate.y = std::max(gameObject_.transformData.translate.y, 0.0f);
	//地面にいるかどうか
	if (gameObject_.transformData.translate.y <= 0.0f) {
		isOnGround_ = true;
	}

	//攻撃
	Attack();

	//マテリアルのセット
	gameObject_.object3d->GetModel()->SetMaterial(gameObject_.material);

	//リムライトのセット
	gameObject_.object3d->GetModel()->SetRimLight(rimLight_);

	//ヘッドライトの更新
	HeadlightUpdate();

	//視点
	LookDirection();

	//トランスフォームを設定
	gameObject_.object3d->SetTransformData(gameObject_.transformData);

	//3Dオブジェクトの更新
	gameObject_.object3d->Update();

	//ヒットボックスの更新
	gameObject_.hitBox->SetTranslate(gameObject_.object3d->GetWorldPos());
	gameObject_.hitBox->SetRotate(gameObject_.transformData.rotate);
	gameObject_.hitBox->SetScale(hitBoxScale_);
	gameObject_.hitBox->Update();

	//HP
	hpBar_->SetTransformData(hpBarTransform_);
	hpBar_->SetColor(hpColor_);
	hpBar_->Update();
	hpOutLine_->SetTransformData(hpOutLineTransform_);
	hpOutLine_->Update();
}

//描画
void Player::Draw() {
	//3Dオブジェクトの描画
	gameObject_.object3d->Draw();
	//弾の描画
	bullet_->Draw();
	//ヒットボックスの描画
	gameObject_.hitBox->Draw();
	//HP
	hpBar_->Draw();
	hpOutLine_->Draw();
}

//デバッグ用
void Player::Debug() {
#ifdef USE_IMGUI
	ImGui::DragFloat3("direction", &gameObject_.direction.x, 0.1f);
	ImGui::DragFloat3("acceleration", &gameObject_.acceleration.x, 0.1f);
	ImGui::DragFloat3("velocity", &gameObject_.velocity.x, 0.1f);
	ImGui::DragFloat3("trasnalate", &gameObject_.transformData.translate.x, 0.1f);
	ImGui::Checkbox("isOnGround", &isOnGround_);
#endif // USE_IMGUI
}

//終了
void Player::Finalize() {
	//3Dオブジェクトの解放
	delete gameObject_.object3d;
	//弾の解放
	delete bullet_;
	//ヒットボックスの解放
	delete gameObject_.hitBox;
	//スプライトの開放
	delete hpBar_;
	delete hpOutLine_;
}

//衝突したら
void Player::OnCollision() {
	//攻撃を受ける
	Damage();
}

//カメラのセッター
void Player::SetCamera(Camera* camera) {
	gameObject_.object3d->SetCamera(camera);
	gameObject_.hitBox->SetCamera(camera);
}

//位置のセッター
void Player::SetPosition(const Vector3& positBulletion) {
	gameObject_.transformData.translate = positBulletion;
}

//速度のセッター
void Player::SetVelocity(const Vector3& velocity) {
	gameObject_.velocity = velocity;
}

//地面の上にいるかのフラグのセッター
void Player::SetIsOnGround(bool isOnGround) {
	isOnGround_ = isOnGround;
}

//地面の上にいるかのフラグのゲッター
bool Player::IsOnGround() {
	return isOnGround_;
}

//オブジェクト3dのゲッター
Vector3 Player::GetWorldPos() const {
	return gameObject_.object3d->GetWorldPos();
}

//トランスフォームデータのゲッター
TransformData Player::GetTransformData()const {
	return gameObject_.transformData;
}

//速度のゲッター
Vector3 Player::GetVelocity() const {
	return gameObject_.velocity;
}

//OBBのゲッター
OBB Player::GetOBB()const {
	return gameObject_.hitBox->GetOBB();
}

//弾のゲッター
Bullet* Player::GetBullet()const {
	return bullet_;
}

//生存フラグのゲッター
bool Player::IsAlive() {
	return gameObject_.isAlive;
}

//移動
void Player::Move() {
#ifdef _DEBUG
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
#endif // _DEBUG

	//XboxPadの平行移動
	if (input_->IsXboxPadConnected(xBoxPadNumber_)) {
		if (std::fabs(input_->GetXboxPadLeftStick(xBoxPadNumber_).x) > 0.0f
			|| std::fabs(input_->GetXboxPadLeftStick(xBoxPadNumber_).y) > 0.0f) {
			gameObject_.direction.x = input_->GetXboxPadLeftStick(xBoxPadNumber_).x;
			gameObject_.direction.z = input_->GetXboxPadLeftStick(xBoxPadNumber_).y;
		} else {
			gameObject_.direction.x = 0.0f;
			gameObject_.direction.z = 0.0f;
		}
	}

	//カメラを移動させる
	gameObject_.velocity.x = gameObject_.direction.Normalize().x * kMoveSpeed;
	gameObject_.velocity.z = gameObject_.direction.Normalize().z * kMoveSpeed;
}

//攻撃
void Player::Attack() {
	//弾の発射
	bullet_->SetShootingPosition(gameObject_.object3d->GetWorldPos());
	bullet_->SetSourceWorldMatrix(gameObject_.object3d->GetWorldTransform()->GetWorldMatrix());
	bullet_->Fire(input_->TriggerXboxPad(xBoxPadNumber_, XboxInput::kRT));
	bullet_->Update();
}

//ジャンプ
void Player::Jump() {
	if (isOnGround_) {
		//地面にいたらジャンプできるようにする
		if (input_->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
			//Y軸に初速を代入
			gameObject_.velocity.y = kJumpSpeed;
			//地面にいるかどうかのフラグをfalse
			isOnGround_ = false;
		}

#ifdef _DEBUG
		if (input_->TriggerKey(DIK_SPACE)) {
			//Y軸に初速を代入
			gameObject_.velocity.y = kJumpSpeed;
			//地面にいるかどうかのフラグをfalse
			isOnGround_ = false;
		}
#endif // _DEBUG

	}
}

//速度と加速度を位置に適応
void Player::IntegrateMotion() {
	//加速度を適応
	gameObject_.velocity += gameObject_.acceleration * Math::kDeltaTime;
	//速度を適応
	gameObject_.transformData.translate += gameObject_.velocity * Math::kDeltaTime;
}

//ダメージを受ける
void Player::Damage() {
	//ダメージクールタイムが0だったら
	if (damageCoolTime_ <= 0.0f) {
		//Hpバーに適応
		hpBarTransform_.scale.x -= hpBarWidth_ / static_cast<float>(kMaxHpCount);
		//ダメージクールタイムを設定
		damageCoolTime_ = kMaxDamageCoolTime;
	}
}

//死亡
void Player::Dead() {
	if (hpBarTransform_.scale.x <= 0) {
		gameObject_.isAlive = false;
	}
}

//ヘッドライトの更新
void Player::HeadlightUpdate() {
	//ライトの位置をプレイヤーの前に設定
	Vector3 headlightOffset = { 0.0f, 0.0f, 0.5f };
	headlight_.position = gameObject_.object3d->GetWorldPos() + headlightOffset;
	// ローカル空間での“前”の向き
	Vector3 localForward = { 0.0f, 0.0f, 1.0f };

	//プレイヤーのワールド行列を取得
	Matrix4x4 world = gameObject_.object3d->GetWorldTransform()->GetWorldMatrix();

	//前方向ベクトルだけをワールド空間に変換
	Vector3 worldForward = Math::TransformNormal(localForward, world);

	//正規化してライトの方向に設定
	headlight_.direction = worldForward.Normalize();
	//headlight_.intensity = 30.0f;
	//ライトのセッター
	object3dCommon_->SetSpotLightList(0, headlight_);
}

//視点
void Player::LookDirection() {
	// XboxPad
	if (input_->IsXboxPadConnected(xBoxPadNumber_)) {

		float stickX = input_->GetXboxPadRighttStick(xBoxPadNumber_).x; // -1..1

		// デッドゾーン（スティックの微妙なブレ対策）
		const float deadZone = 0.15f;
		if (std::fabs(stickX) < deadZone) {
			stickX = 0.0f;
		} else {
			// デッドゾーン分を詰めて 0..1 に戻す（好み）
			stickX = (stickX - copysignf(deadZone, stickX)) / (1.0f - deadZone);
		}

		// 回転速度（ラジアン/秒） 例：180度/秒
		const float yawSpeed = Math::kPi; // PI rad/s = 180deg/s

		gameObject_.transformData.rotate.y += stickX * yawSpeed * Math::kDeltaTime;
	}

	// 必要なら 0..2PI に丸める（巨大化防止）
	const float twoPi = Math::kPi * 2.0f;
	if (gameObject_.transformData.rotate.y > twoPi) gameObject_.transformData.rotate.y -= twoPi;
	if (gameObject_.transformData.rotate.y < 0.0f)  gameObject_.transformData.rotate.y += twoPi;

	gameObject_.object3d->SetRotate(gameObject_.transformData.rotate);
}
