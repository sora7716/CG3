#pragma once
#include "engine/math/Vector3.h"
#include "engine/math/Vector2.h"

//前方宣言
class Camera;
class Input;

/// <summary>
/// デバックカメラ
/// </summary>
class DebugCamera {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DebugCamera() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DebugCamera() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラのゲッター
	/// </summary>
	/// <returns>カメラ</returns>
	Camera* GetCamera();

	void Debug();

private://メンバ関数
	/// <summary>
	/// X軸ローカル移動 
	/// </summary>
	void TranslateLocalX();

	/// <summary>
	/// Y軸ローカル移動
	/// </summary>
	void TranslateLocalY();

	/// <summary>
	/// Z軸ローカル移動
	/// </summary>
	void TranslateLocalZ();

	/// <summary>
	/// 平行移動の更新
	/// </summary>
	void UpdateTranslate();

	/// <summary>
	/// 回転の更新
	/// </summary>
	void UpdateRotate();

public://定数
	//カメラ回転時のマウスの移動量に対する回転角の倍率
	static inline const float kLookRadPerCount = 1.0f / 1000.0f;
private://メンバ変数
	//カメラ
	Camera* camera_ = nullptr;
	//X,Y,Z軸回りのローカル回転角
	Vector3 rotate_ = { 0.0f,0.0f,0.0f };
	//ローカル座標
	Vector3 translate_ = { 0.0f,0.0f,-10.0f };
	//マウスのフリック量
	Vector2 mouseFlick_ = {};
	//カメラの移動方向のベクトル
	Vector3 moveDir_ = {};
	//カメラの移動速度
	float speed_ = 0.1f;
	//入力
	Input* input_ = nullptr;
};

