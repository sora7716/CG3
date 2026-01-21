#include "Player.h"
#include "engine/3d/Object3d.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/camera/Camera.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/input/Input.h"
#include "engine/math/func/Math.h"
#include "engine/worldTransform/WorldTransform.h"
#include "Bullet.h"
#include "engine/debug/WireframeObject3d.h"
#include "engine/scene/SceneManager.h"
#include "engine/2d/Sprite.h"

//デストラクタ
Player::~Player() {
	Finalize();
}

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
	gameObject_.object3d->Initialize(camera_);
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
	bullet_->SetAliveRange(kAliveAreaSize);
	bullet_->SetSpeed(kBulletSpeed);
	bullet_->SetSize({ kBulletSize,kBulletSize,kBulletSize });
	bullet_->SetMaxBulletCount(kBulletCount);

	//スポットライトを設定
	Object3dCommon::GetInstance()->AddSpotLight("headlight");
	headlight_ = Object3dCommon::GetInstance()->GetSpotLight("headlight");
	headlight_.cosAngle = 0.9f;
	headlight_.cosFolloffStart = 1.2f;

	//ヒットボックス
	gameObject_.hitBox = new WireframeObject3d();
	gameObject_.hitBox->Initialize(camera_, ModelType::kCube);

	//HPバー
	//中身
	hpBar_ = new Sprite();
	hpBar_->Initialize("playerHpBar.png");
	hpBar_->SetBlendMode(BlendMode::kNormal);
	hpBarTransform_.scale = { hpBarWidth_,70.0f };
	hpBarTransform_.translate = { 440.0f,630.0f };
	hpColor_ = Vector4::ColorCodeTransform("#094206FF");
	//アウトライン
	hpOutLine_ = new Sprite();
	hpOutLine_->Initialize("playerHpOutLine.png");
	hpOutLine_->SetBlendMode(BlendMode::kNormal);
	hpOutLineTransform_.scale = { hpBarWidth_,70.0f };
	hpOutLineTransform_.translate = { 440.0f,630.0f };

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

	//3Dオブジェクトの更新
	gameObject_.object3d->Update();

	//ヒットボックスの更新
	gameObject_.hitBox->SetTranslate(gameObject_.object3d->GetWorldPos());
	gameObject_.hitBox->SetRotate(gameObject_.transformData.rotate);
	gameObject_.hitBox->SetScale(hitBoxScale_);
	gameObject_.hitBox->Update();

	//HP
	hpBar_->SetTransform(hpBarTransform_);
	hpBar_->SetColor(hpColor_);
	hpBar_->Update();
	hpOutLine_->SetTransform(hpOutLineTransform_);
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
	ImGui::Text("hp:%d", hp_);
	/*ImGui::DragFloat3("rotate", &gameObject_.transformData.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &gameObject_.transformData.translate.x, 0.1f);
	ImGui::ColorEdit4("rimColor", &rimLight_.color.x);
	ImGui::DragFloat("rimPower", &rimLight_.power, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("rimSoftness", &rimLight_.softness, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("rimOutLinePower", &rimLight_.outLinePower, 0.1f, 0.0f, 100.0f);
	ImGuiManager::GetInstance()->CheckBoxToInt("enableRimLighting", rimLight_.enableRimLighting);
	ImGui::DragFloat("cosAngle", &headlight_.cosAngle, 0.1f);
	ImGui::DragFloat("cosFolloffStart", &headlight_.cosFolloffStart, 0.1f);
	ImGui::DragFloat3("hitBox.scale", &hitBoxScale_.x, 0.1f);*/
	ImGui::DragFloat2("hp.scale", &hpBarTransform_.scale.x, 0.1f);
	ImGui::DragFloat2("hp.translate", &hpBarTransform_.translate.x, 0.1f);
	ImGui::ColorEdit4("hp.color", &hpColor_.x);
	ImGui::DragFloat2("hpOutLine.scale", &hpOutLineTransform_.scale.x, 0.1f);
	ImGui::DragFloat2("hpOutLine.translate", &hpOutLineTransform_.translate.x, 0.1f);
#endif // USE_IMGUI
}

//終了
void Player::Finalize() {
	//3Dオブジェクトの解放
	delete gameObject_.object3d;
	gameObject_.object3d = nullptr;
	//弾の解放
	delete bullet_;
	bullet_ = nullptr;
	//ヒットボックスの解放
	delete gameObject_.hitBox;
	gameObject_.hitBox = nullptr;
	//スプライトの開放
	delete hpBar_;
	hpBar_ = nullptr;
	delete hpOutLine_;
	hpOutLine_ = nullptr;
}

//衝突したら
void Player::OnCollision() {
	if (hp_ < 0) {
		//SceneManager::GetInstance()->ChangeScene("Result");
	} else {
		hp_--;
		//Hpバーに適応
		hpBarTransform_.scale.x -= hpBarWidth_ / static_cast<float>(kMaxHpCount);
	}
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
	//gameObject_.transformData.translate += (gameObject_.velocity * gameObject_.moveDirection.Normalize()) * Math::kDeltaTime;

	////トランスフォームのセット
	//gameObject_.object3d->SetTransform(gameObject_.transformData);

	//XboxPadの平行移動
	if (input_->IsXboxPadConnected(xBoxPadNumber_)) {
		if (std::fabs(input_->GetXboxPadLeftStick(xBoxPadNumber_).x) > 0.0f
			|| std::fabs(input_->GetXboxPadLeftStick(xBoxPadNumber_).y) > 0.0f) {
			gameObject_.moveDirection.x = input_->GetXboxPadLeftStick(xBoxPadNumber_).x;
			gameObject_.moveDirection.z = input_->GetXboxPadLeftStick(xBoxPadNumber_).y;
		}
	}

	//プレイヤーの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Rendering::MakeRotateXYZMatrix(gameObject_.transformData.rotate);

	//カメラの向いてる方向を正にする(XとZ軸限定)
	Vector3 moveDirXZ = Math::TransformNormal(Vector3(gameObject_.moveDirection.x, 0.0f, gameObject_.moveDirection.z), rotMat);

	//Y軸のそのまま
	gameObject_.moveDirection = { moveDirXZ.x,gameObject_.moveDirection.y,moveDirXZ.z };

	//カメラを移動させる
	gameObject_.transformData.translate += gameObject_.moveDirection.Normalize() * kMoveSpeed * Math::kDeltaTime;
	//トランスフォームのセット
	gameObject_.object3d->SetTransformData(gameObject_.transformData);
}

//攻撃
void Player::Attack() {
	//弾の発射
	bullet_->SetShootingPosition(gameObject_.object3d->GetWorldPos());
	bullet_->SetSourceWorldMatrix(gameObject_.object3d->GetWorldTransform()->GetWorldMatrix());
	bullet_->Fire(input_->TriggerXboxPad(xBoxPadNumber_, XboxInput::kRT));
	bullet_->Update();
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
	headlight_.intensity = 30.0f;
	//ライトのセッター
	Object3dCommon::GetInstance()->SetSpotLight("headlight", headlight_);
}
