#include "EnemyManager.h"
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
void EnemyManager::Initialize() {
	enemyList_.resize(kMaxEnemy);
}

//終了
void EnemyManager::Finalize() {
	//インスタンスの解放
	delete instance;
	instance = nullptr;
	isFinalize = true;
}
