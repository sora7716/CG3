#pragma once
#include "ActorData.h"
#include <Windows.h>
#include <string>

// 前方宣言
class Camera;
class Input;
class WorldTransform;
class Bullet;

/// <summary>
/// プレイヤー
/// </summary>
class Player {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player() = default;

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
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ用
	/// </summary>
	void Debug();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// 位置のセッター
	/// </summary>
	/// <param name="position">位置</param>
	void SetPosition(const Vector3& position);

	/// <summary>
	/// ワールド座標系での位置のゲッター
	/// </summary>
	/// <returns>ワールド座標系での位置</returns>
	Vector3 GetWorldPos();

	/// <summary>
	/// トランスフォームデータのゲッター
	/// </summary>
	/// <returns>トランスフォームデータ</returns>
	TransformData GetTransformData();

	/// <summary>
	/// 速度のゲッター
	/// </summary>
	/// <returns>速度</returns>
	Vector3 GetVelocity();
private://定数
	//カメラの移動速度
	static inline const float kMoveSpeed = 4.0f;
private://メンバ関数
	/// <summary>
	/// 移動
	/// </summary>
	void Move();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// ヘッドライトの更新
	/// </summary>
	void HeadlightUpdate();
private://メンバ変数
	//入力
	Input* input_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	//プレイヤーデータ
	GameObject gameObject_ = {};

	//弾
	Bullet* bullet_ = nullptr;

	//ヘッドライト
	SpotLightData headlight_ = {};

	//リムライト
	RimLight rimLight_ = {};

	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
};

