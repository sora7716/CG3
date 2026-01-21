#pragma once
#include <string>
#include "ActorData.h"
#include "engine/math/RenderingData.h"
#include "EnemyState.h"
#include "engine/math/CollisionPrimitives.h"

//前方宣言
class Object3d;
class Camera;
class WireframeObject3d;
class Bullet;

/// <summary>
/// 敵
/// </summary>
class Enemy {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Enemy() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="modelName">モデル名</param>
	void Initialize(Camera* camera, const std::string& modelName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 衝突したら
	/// </summary>
	void OnCollision();

	/// <summary>
	/// 待機
	/// </summary>
	void Idol();

	/// <summary>
	/// 追従
	/// </summary>
	void Chase();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera*camera);

	/// <summary>
	/// ターゲットの位置のセッター
	/// </summary>
	/// <param name="targetPos">ターゲットの位置</param>
	void SetTarget(const Vector3& targetPos);

	/// <summary>
	/// 平行移動のセッター
	/// </summary>
	/// <param name="translate">平行移動</param>
	void SetTranslate(const Vector3& translate);

	/// <summary>
	/// 弾のゲッター
	/// </summary>
	/// <returns>弾</returns>
	Bullet* GetBullet()const;

	/// <summary>
	/// OBBのゲッター
	/// </summary>
	/// <returns>OBB</returns>
	OBB GetOBB()const;
private://メンバ関数
	/// <summary>
	/// ターゲットの方向を向く
	/// </summary>
	void EnemyToTarget();

	/// <summary>
	/// 敵の振る舞い
	/// </summary>
	void Behavior();
private://定数
	//移動速度
	static inline const float kSpeed = 0.1f;
	//待機時の回転速度
	static inline const float kIdolRotSpeed = 0.5f;
	//弾を撃つタイマーのリミット
	static inline const float kBulletShotTimerLimit = 0.5f;
	//攻撃タイマーのリミット
	static inline const float kAttackTimerLimit = 5.0f;
	//弾の数
	static inline const uint32_t kBulletCount = 100;
	//弾の速度
	static inline const float kBulletSpeed = -8.0f;
	//弾の大きさ
	static inline const float kBulletSize = 0.25f;
	//生存エリアのサイズ
	static inline const float kAliveAreaSize = 100.0f;
private://メンバ変数
	//ゲームオブジェクト
	GameObject gameObject_ = {};
	
	//ターゲットの位置
	Vector3 targetPos_ = {};
	
	//敵の状態
	IEnemyState* enemyState_ = nullptr;

	//弾
	Bullet* bullet_ = nullptr;
	//弾を撃つタイマー
	float bulletShotTimer_ = 0.0f;
	//攻撃フラグ
	bool isAttacking_ = false;
	//攻撃タイマー
	float attackTimer_ = 0.0f;

	//ワイヤーフレーム
	//球
	WireframeObject3d* sphere_ = nullptr;
	float sphereRadius_ = 0.0f;
	//行動が変化するエリア
	WireframeObject3d* attackArea = nullptr;
	float behaviorAreaRadius_ = 0.0f;

	//ヒットボックス
	Vector3 hitBoxScale_ = { 1.0f,1.0f,1.0f };

	//ヒットポイント
	int32_t hp_ = 5;
};

