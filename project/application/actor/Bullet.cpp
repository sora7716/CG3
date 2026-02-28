#include "Bullet.h"
#include "engine/3d/Object3d.h"
#include "engine/camera/Camera.h"
#include "engine/math/func/Math.h"
#include "engine/debug/ImGuiManager.h"
#include "WireframeObject3d.h"
#include "engine/3d/Object3dCommon.h"

//デストラクタ
Bullet::~Bullet() {
	Finalize();
}

//初期化
void Bullet::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	//3Dオブジェクトの共通部分を記録
	object3dCommon_ = object3dCommon;

	//カメラを記録
	camera_ = camera;

	//ヒットボックスのサイズを設定
	hitBoxScale_ = { 0.25f,0.25f,0.25f };

	//弾の最大数を設定
	bulletDatas_.resize(kMaxBulletCount);

	for (BulletData& bullet : bulletDatas_) {
		bullet.gameObject.object3d = new Object3d();
		bullet.gameObject.object3d->Initialize(object3dCommon_, camera);
		bullet.gameObject.hitBox = new WireframeObject3d();
		bullet.gameObject.hitBox->Initialize(object3dCommon_->GetWireframeObject3dCommon(), camera_, ModelType::kCube);
	}
}

//弾を発射する
void Bullet::Fire(bool isShooting) {
	//弾を発射しない場合は早期リターン
	if (!isShooting) {
		return;
	}

	//弾を生成
	for (BulletData& bullet : bulletDatas_) {
		if (!bullet.gameObject.isAlive) {
			CreateBullet(bullet);
			break;
		}
	}
}

//更新
void Bullet::Update() {
	for (BulletData& bulletData : bulletDatas_) {
		if (bulletData.gameObject.isAlive) {
			//飛ばす
			bulletData.gameObject.transformData.translate += bulletData.gameObject.velocity;

			//弾の情報を設定
			bulletData.gameObject.object3d->SetTransformData(0,bulletData.gameObject.transformData);
			bulletData.gameObject.object3d->GetModel()->SetMaterial(bulletData.gameObject.material);

			//弾の生存させるか
			Vector3 currentPos = bulletData.gameObject.transformData.translate;
			float length = (currentPos - bulletData.shootingPoint).Length();
			if (bulletData.aliveRange < length) {
				bulletData.gameObject.isAlive = false;
				continue;
			}

			//弾の更新
			bulletData.gameObject.object3d->Update();

			//ヒットボックスの更新
			bulletData.gameObject.hitBox->SetTranslate(0,bulletData.gameObject.object3d->GetWorldPos(0));
			bulletData.gameObject.hitBox->SetRotate(0,bulletData.gameObject.transformData.rotate);
			bulletData.gameObject.hitBox->SetScale(0,hitBoxScale_);
			bulletData.gameObject.hitBox->Update();
		}
	}
}

//衝突したときの
void Bullet::OnCollision(uint32_t index) {
	bulletDatas_[index].gameObject.isAlive = false;
}

//描画
void Bullet::Draw() {
	//弾の描画
	for (const BulletData& bulletData : bulletDatas_) {
		if (bulletData.gameObject.isAlive) {
			bulletData.gameObject.object3d->Draw();
			bulletData.gameObject.hitBox->Draw();
		}
	}
}

//終了
void Bullet::Finalize() {
	//弾のデータ
	for (BulletData& bulletData : bulletDatas_) {
		delete bulletData.gameObject.object3d;
		bulletData.gameObject.object3d = nullptr;
		delete bulletData.gameObject.hitBox;
		bulletData.gameObject.hitBox = nullptr;
	}
	bulletDatas_.clear();
}

//発射地点のセッター
void Bullet::SetShootingPosition(const Vector3& shootingPosition) {
	shootingPosition_ = shootingPosition;
}

//弾を保持しているオブジェクトのワールド行列セッター
void Bullet::SetSourceWorldMatrix(const Matrix4x4& sourceWorldMatrix) {
	sourceWorldMatrix_ = sourceWorldMatrix;
}

//スピードのセッター
void Bullet::SetSpeed(float speed) {
	speed_ = speed;
}

//サイズのセッター
void Bullet::SetSize(const Vector3& size) {
	size_ = size;
}

//角度のセッター
void Bullet::SetAngle(const Vector3& angle) {
	angle_ = angle;
}

//生存範囲のセッター
void Bullet::SetAliveRange(float aliveRange) {
	aliveRange_ = aliveRange;
}

//弾の発射できる最大値のセッター
void Bullet::SetMaxBulletCount(uint32_t maxBulletCount) {
	maxBulletCount_ = maxBulletCount;
}

//モデルのセッター
void Bullet::SetModel(const std::string& modelName) {
	modelName_ = modelName;
}

//弾のデータリストのゲッター
std::vector<BulletData>& Bullet::GetBulletData() {
	return bulletDatas_;
}

//弾の生成
void Bullet::CreateBullet(BulletData&bulletData) {
	bulletData.gameObject.transformData.scale = Vector3::MakeAllOne();
	bulletData.gameObject.isAlive = true;
	//弾の位置を設定
	bulletData.gameObject.transformData.translate = shootingPosition_;
	//射撃地点を取得
	bulletData.shootingPoint = bulletData.gameObject.transformData.translate;

	//マテリアルの初期化
	bulletData.gameObject.material.color = { 1.0f,1.0f,1.0f,1.0f };
	bulletData.gameObject.material.enableLighting = true;
	bulletData.gameObject.material.shininess = 10.0f;
	bulletData.gameObject.material.uvMatrix = Matrix4x4::Identity4x4();

	//3Dモデルの生成
	bulletData.aliveRange = aliveRange_;
	bulletData.gameObject.object3d->SetModel(modelName_);

	//サイズと角度の設定
	bulletData.gameObject.transformData.scale = size_;
	bulletData.gameObject.transformData.rotate = angle_;

	//弾の速度を設定
	bulletData.gameObject.velocity = { 0.0f,0.0f,speed_ };
	bulletData.direction = Math::TransformNormal(bulletData.gameObject.velocity, sourceWorldMatrix_);
	bulletData.gameObject.velocity = bulletData.direction * Math::kDeltaTime;
}