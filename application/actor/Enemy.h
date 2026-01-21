#pragma once
#include <string>
#include "ActorData.h"
#include "engine/math/RenderingData.h"
#include "EnemyState.h"
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
private://メンバ関数
	/// <summary>
	/// ターゲットの方向を向く
	/// </summary>
	void EnemyToTarget();
private://メンバ変数
	//ゲームオブジェクト
	GameObject gameObject_ = {};
	//ターゲットの位置
	Vector3 targetPos_ = {};
	
	//敵の状態
	IEnemyState* enemyState_ = nullptr;

	//弾
	Bullet* bullet_ = nullptr;

	//ワイヤーフレーム
	//球
	WireframeObject3d* sphere_ = nullptr;
	float sphereRadius_ = 0.0f;

	WireframeObject3d* behaviorArea_ = nullptr;
	float behaviorAreaRadius_ = 0.0f;
};

