#include "Bullet.h"
#include "engine/3d/Object3d.h"
#include "engine/camera/Camera.h"
#include "engine/math/func/Math.h"
#include "engine/debug/ImGuiManager.h"

//初期化
void Bullet::Initialize(Camera* camera) {
	//カメラを記録
	camera_ = camera;
}

//弾を発射する
void Bullet::Fire(bool isShooting) {
	//弾を発射しない場合は早期リターン
	if (!isShooting) {
		return;
	}

	//弾の最大値を超えた場合早期リターン
	if (bulletDataList_.size() > maxBulletCount_) {
		return;
	}

	//弾を生成
	bulletDataList_.push_back(CreateBullet());
}

//更新
void Bullet::Update() {
	for (BulletData& bulletData : bulletDataList_) {
		if (bulletData.gameObject.isAlive) {
			//飛ばす
			bulletData.gameObject.transformData.translate += bulletData.gameObject.velocity;

			//弾の情報を設定
			bulletData.gameObject.object3d->SetTransform(bulletData.gameObject.transformData);
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
		}
	}

	//弾の削除
	for (BulletData& bulletData: bulletDataList_) {
		if (!bulletData.gameObject.isAlive) {
			delete bulletData.gameObject.object3d;
			bulletData.gameObject.object3d = nullptr;
		}
	}

	//リストから削除
	bulletDataList_.remove_if([](const BulletData& b) {
		return !b.gameObject.isAlive;
	});
}

//描画
void Bullet::Draw() {
	//弾の描画
	for (const BulletData& bulletData : bulletDataList_) {
		if (bulletData.gameObject.isAlive) {
			bulletData.gameObject.object3d->Draw();
		}
	}
}

//終了
void Bullet::Finalize() {
	for (BulletData& bulletData : bulletDataList_) {
		delete bulletData.gameObject.object3d;
		bulletData.gameObject.object3d = nullptr;
	}
	bulletDataList_.clear();
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

//弾の生成
BulletData Bullet::CreateBullet() {
	BulletData bullet = {};
	bullet.gameObject.transformData.scale = Vector3::MakeAllOne();
	bullet.gameObject.isAlive = true;
	//弾の位置を設定
	bullet.gameObject.transformData.translate = shootingPosition_;
	//射撃地点を取得
	bullet.shootingPoint = bullet.gameObject.transformData.translate;

	//マテリアルの初期化
	bullet.gameObject.material.color = { 1.0f,1.0f,1.0f,1.0f };
	bullet.gameObject.material.enableLighting = true;
	bullet.gameObject.material.shininess = 10.0f;
	bullet.gameObject.material.uvMatrix = Matrix4x4::Identity4x4();

	//3Dモデルの生成
	bullet.gameObject.object3d = new Object3d();
	bullet.gameObject.object3d->Initialize(camera_);
	bullet.aliveRange = aliveRange_;
	bullet.gameObject.object3d->SetModel("bullet");

	//サイズと角度の設定
	bullet.gameObject.transformData.scale = size_;
	bullet.gameObject.transformData.rotate = angle_;

	//弾の速度を設定
	bullet.gameObject.velocity = { 0.0f,0.0f,speed_ };
	bullet.direction = Math::TransformNormal(bullet.gameObject.velocity, sourceWorldMatrix_);
	bullet.gameObject.velocity = bullet.direction * Math::kDeltaTime;
	return bullet;
}