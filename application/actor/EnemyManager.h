#pragma once
#include "Enemy.h"
#include <list>
#include <random>

//前方宣言
class Object3dCommon;
class Camera;

/// <summary>
/// 敵の管理
/// </summary>
class EnemyManager {
public://メンバ関数
	/// <summary>
	/// インスタンスのゲッター
	/// </summary>
	/// <returns>インスタンス</returns>
	static EnemyManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">3Dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(Object3dCommon* object3dCommon, Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="target">ターゲット</param>
	void Update(const Vector3& target);

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// スポーンエリアの中心のセッター
	/// </summary>
	/// <param name="spawnAreaCenter">スポーンエリアの中心</param>
	void SetSpawnAreaCenter(const Vector3& spawnAreaCenter);

	/// <summary>
	/// 敵のリストを取得
	/// </summary>
	/// <returns>敵のリスト</returns>
	std::list<Enemy*>GetEnemyList();

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

	/// <summary>
	/// 敵を生成
	/// </summary>
	/// <returns></returns>
	Enemy* Create();

	/// <summary>
	/// 敵のスポーン
	/// </summary>
	void Spawn();
private://静的メンバ変数
	//インスタンス
	static inline EnemyManager* instance = nullptr;
	//終了を呼んだかどうか
	static inline bool isFinalize = false;
private://定数
	//敵の数
	static inline const int32_t kMaxEnemy = 10;

	//最初に出てくる敵の数
	static inline const int32_t kFirstSpawnEnemyCount = 2;
private://メンバ変数
	//3Dオブジェクトの共通部分
	Object3dCommon* object3dCommon_ = nullptr;
	//敵
	std::list<Enemy*>enemyList_;
	//カメラ
	Camera* camera_ = nullptr;
	//スポーンできるエリアの中心
	Vector3 spawnAreaCenter_ = {};
	//スポーンできるエリアの半径
	float spawnAreaRadius_ = 10.0f;
	//ランダムエンジン
	std::mt19937 randomEngine_;
	//スポーンタイマー
	float spawnTimer_ = 0;
	//スポーンタイマーのリミット
	float spawnInterval_ = 7;
};

