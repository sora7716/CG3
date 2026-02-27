#define NOMINMAX
#include "Enemy.h"
#include "engine/3d/Object3d.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/math/func/Math.h"
#include "engine/math/func/Rendering.h"
#include "engine/debug/WireframeObject3d.h"
#include "engine/math//func/Collision.h"
#include "Bullet.h"
#include "Score.h"

//デストラクタ
Enemy::~Enemy() {
	Finalize();
}

//初期化
void Enemy::Initialize(Object3dCommon* object3dCommon, Camera* camera, const std::string& modelName) {
	//3Dモデルの生成と初期化
	gameObject_.object3d = new Object3d();
	gameObject_.object3d->Initialize(object3dCommon, camera, TransformMode::k3d);
	gameObject_.object3d->SetModel(modelName);
	gameObject_.transformData.scale = Vector3::MakeAllOne() / 2.0f;
	gameObject_.isAlive = true;
	gameObject_.tag = Tag::kEnemy;

	collider_.owner = &gameObject_;
	collider_.isTrigger = true;
	collider_.isEnabled = true;
	collider_.onCollision = [this](GameObject* other) {this->OnCollision(other); };

	//弾の生成と初期化
	bullet_ = new Bullet();
	bullet_->Initialize(object3dCommon, camera);
	bullet_->SetAliveRange(kAliveAreaSize);
	bullet_->SetSpeed(bulletShotSpeed_);
	bullet_->SetSize({ kBulletSize,kBulletSize,kBulletSize });
	bullet_->SetMaxBulletCount(kBulletCount);

	//敵の状態を生成
	enemyState_ = new EnemyStateChase();
	enemyState_->SetEnemy(this);

	//ワイヤーフレームの生成
	sphere_ = new WireframeObject3d();
	sphere_->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kSphere);
	sphereRadius_ = 2.0f;

	attackArea = new WireframeObject3d();
	attackArea->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kSphere);
	attackAreaRadius_ = 3.0f;
	attackArea->SetTranslate(targetPos_);
	attackArea->Update();

	gameObject_.hitBox = new WireframeObject3d();
	gameObject_.hitBox->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kCube);
	hitBoxScale_ = { 1.5f,1.5f,1.5f };

	//HP
	hpBar_ = new Object3d();
	hpBar_->Initialize(object3dCommon, camera, TransformMode::kBilboard);
	hpBar_->SetModel("hpBar");
	hpBar_->SetTexture("playerHpBar.png");
	hpBarTransform_.scale = { hpBarWidth_,0.2f,1.0f };
	Material hpMaterial;
	hpMaterial.color = Vector4::MakeRedColor();
	hpMaterial.enableLighting = false;
	hpBar_->GetModel()->SetMaterial(hpMaterial);

	Material hpOutLineMaterial;
	hpOutLineMaterial.color = Vector4::MakeWhiteColor();
	hpOutLineMaterial.enableLighting = false;

	hpOutLine_ = new Object3d();
	hpOutLine_->Initialize(object3dCommon, camera, TransformMode::kBilboard);
	hpOutLine_->SetModel("hpOutLine");
	hpOutLine_->SetTexture("playerHpOutLine.png");
	hpOutLine_->GetModel()->SetMaterial(hpOutLineMaterial);
	hpOutLineTransform_.scale = { hpBarWidth_,0.2f,1.0f };
}

//更新
void Enemy::Update() {
	//オブジェクト3Dの更新
	gameObject_.object3d->SetTransformData(gameObject_.transformData);
	gameObject_.object3d->Update();

	//ワイヤーフレームの更新
	//球
	sphere_->SetRadius(sphereRadius_);
	sphere_->SetTranslate(gameObject_.transformData.translate);
	sphere_->Update();

	//このエリアに敵が入ったら動きが変わる
	attackArea->SetRadius(attackAreaRadius_);
	attackArea->SetTranslate(targetPos_);
	attackArea->Update();

	//振る舞い
	//Behavior();

	//弾の更新
	bullet_->Update();

	//ヒットボックス
	gameObject_.hitBox->SetTranslate(gameObject_.object3d->GetWorldPos());
	gameObject_.hitBox->SetRotate(gameObject_.transformData.rotate);
	gameObject_.hitBox->SetScale(hitBoxScale_);
	gameObject_.hitBox->Update();

	//HP
	Vector3 worldPos = gameObject_.object3d->GetWorldPos();
	hpOutLineTransform_.translate = { worldPos.x,worldPos.y + 2.0f,worldPos.z };
	hpOutLine_->SetTransformData(hpOutLineTransform_);
	hpOutLine_->Update();
	hpBarTransform_.translate = { worldPos.x + hpBarPosX_,worldPos.y + 2.0f,worldPos.z };
	hpBar_->SetTransformData(hpBarTransform_);
	hpBar_->Update();
}

//デバッグ
void Enemy::Debug() {
#ifdef _DEBUG
	//ImGuiManager::GetInstance()->DragTransform(gameObject_.transformData);
	//ImGui::DragFloat3("hitBox.scale", &hitBoxScale_.x, 0.1f);
	ImGui::Text("hp:%d", hp_);
	ImGui::DragFloat3("hp.translate", &hpBarTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("hp.scale", &hpBarTransform_.scale.x, 0.1f);
	ImGui::DragFloat("hp.posX", &hpBarPosX_, 0.1f);
	ImGui::DragFloat3("hpOutLine.translate", &hpOutLineTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("hpOutLine.scale", &hpOutLineTransform_.scale.x, 0.1f);
#endif // _DEBUG
}

//描画
void Enemy::Draw() {
	//敵の描画
	gameObject_.object3d->Draw();

	//弾の描画
	bullet_->Draw();

	//ヒットボックスの描画
	gameObject_.hitBox->Draw();

	//ワイヤーフレームの描画
	//sphere_->Draw();

	//敵がこのエリアに入ったら動きが変わる
	//attackArea->Draw();

	//HP
	hpBar_->Draw();
	hpOutLine_->Draw();
}

//終了
void Enemy::Finalize() {
	delete gameObject_.object3d;
	gameObject_.object3d = nullptr;
	delete sphere_;
	sphere_ = nullptr;
	delete attackArea;
	attackArea = nullptr;
	delete bullet_;
	bullet_ = nullptr;
	delete gameObject_.hitBox;
	gameObject_.hitBox = nullptr;
	delete hpBar_;
	hpBar_ = nullptr;
	delete hpOutLine_;
	hpOutLine_ = nullptr;
	delete enemyState_;
	enemyState_ = nullptr;
}

//リセット
void Enemy::Reset() {
	gameObject_.transformData.scale = Vector3::MakeAllOne() / 2.0f;
	gameObject_.isAlive = true;

	//HP
	hp_ = kMaxHpCout;
	hpBarTransform_.scale = { hpBarWidth_,0.2f,1.0f };
	hpBarTransform_.translate = {};
	hpOutLineTransform_.scale = { hpBarWidth_,0.2f,1.0f };
	hpOutLineTransform_.translate = {};
	hpBarPosX_ = 0.0f;
}

//衝突したら
void Enemy::OnCollision(GameObject* other) {
	if (other->tag == Tag::kPlayer) {
		hp_--;
		//描画のHPにも適応
		hpBarTransform_.scale.x -= hpBarWidth_ / kMaxHpCout;
		hpBarPosX_ -= hpBarWidth_ / kMaxHpCout;
		if (hp_ <= 0) {
			gameObject_.isAlive = false;
			//スコアを加算
			Score::AddScore(30);
		}
	}
}

//待機
void Enemy::Idol() {
	gameObject_.transformData.rotate.y += kIdolRotSpeed;
}

//追従
void Enemy::Chase() {
	//ターゲットの方向を向く
	EnemyToTarget();

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Rendering::MakeRotateXYZMatrix(gameObject_.transformData.rotate);
	Vector3 moveDir = { 0.0f,0.0f,-1.0f };
	//カメラの向いてる方向を正にする(XとZ軸限定)
	moveDir = Math::TransformNormal(moveDir, rotMat);
	//カメラを移動させる
	gameObject_.transformData.translate += moveDir.Normalize() * moveSpeed_;
}

//攻撃
void Enemy::Attack() {
	//ターゲットの方向を見る
	EnemyToTarget();

	//攻撃する時間を計測
	if (bulletShotTimer_ < kBulletShotTimerLimit) {
		bulletShotTimer_ += Math::kDeltaTime;
	} else {
		bulletShotTimer_ = 0.0f;
		//攻撃フラグを立てる
		isAttacking_ = true;
	}

	//弾の発射
	bullet_->SetShootingPosition(gameObject_.object3d->GetWorldPos());
	bullet_->SetSourceWorldMatrix(gameObject_.object3d->GetWorldTransform()->GetWorldMatrix());
	bullet_->Fire(isAttacking_);

	//攻撃フラグを折る
	isAttacking_ = false;
}

//カメラのセッター
void Enemy::SetCamera(Camera* camera) {
	gameObject_.object3d->SetCamera(camera);
	sphere_->SetCamera(camera);
	attackArea->SetCamera(camera);
	gameObject_.hitBox->SetCamera(camera);
	hpBar_->SetCamera(camera);
	hpOutLine_->SetCamera(camera);
}

//ターゲットの位置
void Enemy::SetTarget(const Vector3& targetPos) {
	targetPos_ = targetPos;
}

//平行移動のセッター
void Enemy::SetTranslate(const Vector3& translate) {
	gameObject_.transformData.translate = translate;
}

//移動速度のセッター
void Enemy::SetMoveSpeed(float moveSpeed) {
	moveSpeed_ = moveSpeed;
}

//弾の発射速度のセッター
void Enemy::SetBulletShotSpeed(float bulletShotSpeed) {
	bulletShotSpeed_ = bulletShotSpeed;
}

//生存フラグのセッター
void Enemy::SetIsAlive(bool isAlive) {
	gameObject_.isAlive = isAlive;
}

//弾のゲッター
Bullet* Enemy::GetBullet() const {
	return bullet_;
}

//OBBのゲッター
OBB Enemy::GetOBB()const {
	return gameObject_.hitBox->GetOBB();
}

//生存フラグのゲッター
bool Enemy::IsAlive() {
	return gameObject_.isAlive;
}

Collider& Enemy::GetCollider() {
	return collider_;
}

//ターゲットの方向を向く
void Enemy::EnemyToTarget() {
	//プレイヤーの向きに合わせる
	Vector3 dir = (gameObject_.object3d->GetWorldPos() - targetPos_).Normalize();
	float yaw = std::atan2(dir.x, dir.z);
	gameObject_.transformData.rotate.y = yaw;
}

//敵の振る舞い
void Enemy::Behavior() {
	//当たり判定
	if (Collision::IsCollision(sphere_->GetSphere(), attackArea->GetSphere())) {
		//敵の状態を設定
		enemyState_ = new EnemeyStateAttack();

		//ワイヤーフレームの色を変更
		sphere_->SetColor(Vector4::MakeRedColor());
		attackArea->SetColor(Vector4::MakeRedColor());
	} else {

		//攻撃タイマーの加算
		if (attackTimer_ < kAttackTimerLimit) {
			attackTimer_ += Math::kDeltaTime;
		}

		//攻撃タイマーが攻撃タイマーのリミット以上になったら
		if (attackTimer_ >= kAttackTimerLimit) {
			//敵がプレイヤーを追う
			enemyState_ = new EnemyStateChase();
			//攻撃タイマーをリセット
			attackTimer_ = 0.0f;
		}

		//ワイヤーフレームの色を変更
		sphere_->SetColor(Vector4::MakeBlackColor());
		attackArea->SetColor(Vector4::MakeBlackColor());
	}

	//敵の状態
	enemyState_->SetEnemy(this);
	enemyState_->Exce();
}
