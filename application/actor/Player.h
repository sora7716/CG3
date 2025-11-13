#pragma once
#include "ActorData.h"
#include "engine/math/ResourceData.h"
#include <string>
#include <list>

// 前方宣言
class Object3d;
class Camera;
class Input;
class WorldTransform;

//弾
struct Bullet {
	GameObject gameObject;
	Object3d* object3d;
	Vector3 shootingPoint;
	float aliveRange;
	bool isAlive;
};

//プレイヤーデータ
struct PlayerData {
	GameObject gameObject;
	bool isMove;
};

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
	/// オブジェクト3dのゲッター
	/// </summary>
	/// <returns>object3d</returns>
	Object3d* GetObject3d();

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
private://メンバ関数
	/// <summary>
	/// 移動
	/// </summary>
	void Move();

	/// <summary>
	/// 調整項目を適応
	/// </summary>
	void ApplyGlobalVariables();

	/// <summary>
	/// 弾の生成
	/// </summary>
	/// <returns>弾</returns>
	Bullet CreateBullet();
private://メンバ変数
	//入力
	Input* input_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	//3Dオブジェクト
	Object3d* object3d_ = nullptr;

	//プレイヤーデータ
	PlayerData playerData_ = {};

	//マテリアル
	Material material_ = {};

	//弾
	std::list<Bullet> bulletList_ = {};

	//調整項目のグループ名
	const char* groupName_ = "player";
};

