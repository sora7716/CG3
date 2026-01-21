#pragma once
#include "Enemy.h"
#include <vector>

/// <summary>
/// 敵の管理
/// </summary>
class EnemyManager{
public://メンバ関数
	/// <summary>
    /// インスタンスのゲッター
    /// </summary>
    /// <returns>インスタンス</returns>
	static EnemyManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 敵をスポーン
	/// </summary>
	void SpawnEnemy();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();
private://メンバ関数
	//コンストラクタの封印
	EnemyManager() = default;
	//デストラクタの封印
	~EnemyManager() = default;
	//コピーコンストラクタ禁止
	EnemyManager(const EnemyManager&) = delete;
	//代入演算子の禁止
	EnemyManager operator=(const EnemyManager&) = delete;
private://静的メンバ変数
	//インスタンス
	static inline EnemyManager* instance = nullptr;
	//終了を呼んだかどうか
	static inline bool isFinalize = false;
private://定数
	//敵の数
	static inline const int32_t kMaxEnemy = 20;
private://メンバ変数
	//敵
	std::vector<Enemy*>enemyList_;
};

