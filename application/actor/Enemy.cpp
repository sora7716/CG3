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
#include "engine/input/Input.h"

//デストラクタ
Enemy::~Enemy() {
	delete gameObject_.object3d;
	delete sphere_;
	delete behaviorArea_;
	delete bullet_;
}

//初期化
void Enemy::Initialize(Camera* camera, const std::string& modelName) {
	//3Dモデルの生成と初期化
	gameObject_.object3d = new Object3d();
	gameObject_.object3d->Initialize(camera, TransformMode::k3d);
	gameObject_.object3d->SetModel(modelName);
	gameObject_.transformData.scale = Vector3::MakeAllOne();

	//弾の生成
	bullet_ = new Bullet();
	//弾
	bullet_ = new Bullet();
	bullet_->Initialize(camera);
	bullet_->SetAliveRange(200.0f);
	bullet_->SetSpeed(50.0f);
	bullet_->SetSize({ 0.5f,0.5f,0.5f });
	bullet_->SetMaxBulletCount(10);

	//敵の状態を生成
	enemyState_ = new EnemyStateChase();
	enemyState_->Initialize(this);

	//ワイヤーフレームの生成
	sphere_ = new WireframeObject3d();
	sphere_->Initialize(camera, ModelType::kSphere);
	sphereRadius_ = 2.0f;

	behaviorArea_ = new WireframeObject3d();
	behaviorArea_->Initialize(camera, ModelType::kSphere);
	behaviorAreaRadius_ = 5.0f;
}

//更新
void Enemy::Update() {
	//当たり判定
	if (Collision::IsCollision(sphere_->GetSphere(), behaviorArea_->GetSphere())) {
		//敵の状態を設定
		enemyState_ = new EnemeyStateAttack();
		enemyState_->Initialize(this);

		//ワイヤーフレームの色を変更
		sphere_->SetColor(Vector4::MakeRedColor());
		behaviorArea_->SetColor(Vector4::MakeRedColor());
	} else {
		//敵の状態を設定
		enemyState_ = new EnemyStateChase();
		enemyState_->Initialize(this);

		//ワイヤーフレームの色を変更
		sphere_->SetColor(Vector4::MakeBlackColor());
		behaviorArea_->SetColor(Vector4::MakeBlackColor());
	}

	//敵の状態
	enemyState_->Exce();

	//オブジェクト3Dの更新
	gameObject_.object3d->SetTransformData(gameObject_.transformData);
	gameObject_.object3d->Update();

	//ワイヤーフレームの更新
	//球
	sphere_->SetRadius(sphereRadius_);
	sphere_->SetTranslate(gameObject_.transformData.translate);
	sphere_->Update();

	//このエリアに敵が入ったら動きが変わる
	behaviorArea_->SetRadius(behaviorAreaRadius_);
	behaviorArea_->SetTranslate(targetPos_);
	behaviorArea_->Update();

	//弾の発射
	bullet_->SetShootingPosition(gameObject_.object3d->GetWorldPos());
	bullet_->SetSourceWorldMatrix(gameObject_.object3d->GetWorldTransform()->GetWorldMatrix());
	bullet_->Fire(Input::GetInstance()->TriggerKey(DIK_SPACE));
	bullet_->Update();

}

//デバッグ
void Enemy::Debug() {
#ifdef _DEBUG
	ImGuiManager::GetInstance()->DragTransform(gameObject_.transformData);
	ImGui::DragFloat("sphere.radius", &sphereRadius_, 0.1f);
	ImGui::DragFloat("sphere.behaviorAreaRadius_", &behaviorAreaRadius_, 0.1f);
#endif // _DEBUG
}

//描画
void Enemy::Draw() {
	//敵の描画
	gameObject_.object3d->Draw();

	//弾の描画
	bullet_->Draw();

	//ワイヤーフレームの描画
	sphere_->Draw();

	//敵がこのエリアに入ったら動きが変わる
	behaviorArea_->Draw();
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
	gameObject_.transformData.translate += moveDir.Normalize() * 0.01f;
}

//攻撃
void Enemy::Attack() {
	gameObject_.transformData.rotate.y += 0.1f;
}

//カメラのセッター
void Enemy::SetCamera(Camera* camera) {
	gameObject_.object3d->SetCamera(camera);
	sphere_->SetCamera(camera);
	behaviorArea_->SetCamera(camera);
}

//ターゲットの位置
void Enemy::SetTarget(const Vector3& targetPos) {
	targetPos_ = targetPos;
}

//平行移動のセッター
void Enemy::SetTranslate(const Vector3& translate) {
	gameObject_.transformData.translate = translate;
}

//ターゲットの方向を向く
void Enemy::EnemyToTarget() {
	//プレイヤーの向きに合わせる
	Vector3 dir = (gameObject_.object3d->GetWorldPos() - targetPos_).Normalize();
	float yaw = std::atan2(dir.x, dir.z);
	gameObject_.transformData.rotate.y = yaw;
}
