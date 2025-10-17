#pragma once
#include "ActorData.h"
#include "engine/math/ResourceData.h"
#include <string>

// 前方宣言
class Object3d;
class Camera;
class Input;

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
	void Initialize(Camera*camera,const std::string& modelName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ
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
	void SetCamera(Camera*camera);

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
private://メンバ変数
	//入力
	Input* input_ = nullptr;

	//3Dオブジェクト
	Object3d* object3d_ = nullptr;

	//ゲームオブジェクト
	GameObect gameObject_ = {};

	//マテリアル
	Material material_ = {};
};

