#define NOMINMAX
#include "EnemyManager.h"
#include "engine/math/func/Math.h"
#include "engine/debug/ImGuiManager.h"
#include <cassert>
//インスタンスのゲッター
EnemyManager* EnemyManager::GetInstance() {
	assert(!isFinalize && "GetInstance() called after Finalize()");
	if (instance == nullptr) {
		instance = new EnemyManager();
	}
	return instance;

}

//初期化
void EnemyManager::Initialize(Camera* camera) {
	camera_ = camera;
	//ランダムエンジンの初期化
	//乱数エンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator());
	for (int32_t i = 0; i <= kFirstSpawnEnemyCount; i++) {
		//敵を生成
		enemyList_.push_back(Create());
	}
}

//更新
void EnemyManager::Update(const Vector3& target) {
	//敵のスポーン
	Spawn();

	//ターゲットを中心としてエリアを展開
	SetSpawnAreaCenter(target);

	//敵の行動処理
	for (Enemy* enemy : enemyList_) {
		if (enemy->IsAlive()) {
			enemy->SetCamera(camera_);
			enemy->SetTarget(target);
			enemy->Update();
		}
	}

	//リストから削除
	// 死んだやつを delete してからリストから消す
	enemyList_.remove_if([](Enemy* enemy) {
		if (enemy) {
			if (!enemy->IsAlive()) {
				delete enemy;
				return true;   // リストからも消す
			}
		}
		return false;
	});
}

//デバッグ
void EnemyManager::Debug() {
#ifdef _DEBUG
	ImGui::Text("interval:%f", spawnInterval_);
	ImGui::Text("timer:%f", spawnTimer_);
#endif // _DEBUG

}

//描画
void EnemyManager::Draw() {
	for (Enemy* enemy : enemyList_) {
		if (enemy->IsAlive()) {
			enemy->Draw();
		}
	}
}

//リセット
void EnemyManager::Reset() {
	//リストから削除
	// 死んだやつを delete してからリストから消す
	enemyList_.remove_if([](Enemy* enemy) {
		if (enemy) {
			delete enemy;
			return true;   // リストからも消す
		}
		return false;
	});
}

//カメラのセッター
void EnemyManager::SetCamera(Camera* camera) {
	camera_ = camera;
}

//スポーンエリアの中心のセッター
void EnemyManager::SetSpawnAreaCenter(const Vector3& spawnAreaCenter) {
	spawnAreaCenter_ = spawnAreaCenter;
}

//敵のリストを取得
std::list<Enemy*> EnemyManager::GetEnemyList() {
	return enemyList_;
}

//終了
void EnemyManager::Finalize() {
	//敵の解放
	for (Enemy* enemy : enemyList_) {
		delete enemy;
	}
	enemyList_.clear();
	//インスタンスの解放
	delete instance;
	instance = nullptr;
	isFinalize = true;
}

//敵を生成
Enemy* EnemyManager::Create() {
	Enemy* enemy = new Enemy();
	enemy->Initialize(camera_, "enemy");
	//位置の値をemitRange_の範囲でランダムに設定
	Vector3 spawnAreaMin = { -(spawnAreaCenter_.x + spawnAreaRadius_),0.0f,-(spawnAreaCenter_.z + spawnAreaRadius_) };
	Vector3 spawnAreaMax = { spawnAreaCenter_.x + spawnAreaRadius_,0.0f,spawnAreaCenter_.z + spawnAreaRadius_ };
	std::uniform_real_distribution<float>distributionPositionX(spawnAreaMin.x, spawnAreaMax.x);
	std::uniform_real_distribution<float>distributionPositionZ(spawnAreaMin.z, spawnAreaMax.z);
	enemy->SetTranslate({ distributionPositionX(randomEngine_),0.0f,distributionPositionZ(randomEngine_) });

	//移動速度をランダム
	std::uniform_real_distribution<float>distributionMoveSpeed(0.1f, 0.15f);
	enemy->SetMoveSpeed(distributionMoveSpeed(randomEngine_));

	//弾の発射速度をランダム
	std::uniform_real_distribution<float>distributionBulletShotSpeed(-64.0f,-8.0f);
	enemy->SetBulletShotSpeed(distributionBulletShotSpeed(randomEngine_));
	return enemy;
}


//敵をスポーン
void EnemyManager::Spawn() {
	if (enemyList_.size() >= kMaxEnemy) {
		return;
	}

	if (spawnTimer_ < spawnInterval_) {
		spawnTimer_ += Math::kDeltaTime;
	} else {
		//スポーンタイマーをリセット
		spawnTimer_ = 0;
		//敵を生成
		enemyList_.push_back(Create());

		//spawnIntervalを少し減らす
		 // ゆっくり縮める：1回のスポーンで減る量を定数化
		float kIntervalDecreasePerSpawn = 0.15f; // ここを小さくするとゆっくり
		float kMinSpawnInterval = 1.0f;

		spawnInterval_ = std::max(kMinSpawnInterval, spawnInterval_ - kIntervalDecreasePerSpawn);
	}
}