#pragma once
#include "engine/math/Vector3.h"
#include <string>

//前方宣言
class Camera;
class Player;

//移動範囲
struct Rect {
	float left = 0.0f;//左端
	float right = 1.0f;//右端
	float bottom = 0.0f; //下端
	float top = 1.0f;//上端
};

/// <summary>
/// カメラコントローラ
/// </summary>
class CameraController {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CameraController() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~CameraController() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Initialize(Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// 追従対象のセッター
	/// </summary>
	/// <param name="target">追従対象</param>
	void SetTarget(Player* target);

	/// <summary>
	/// 移動範囲のセッター
	/// </summary>
	/// <param name="area">移動範囲</param>
	void SetMovableArea(const Rect& area);
private://定数
	//座標補間割合
	static inline const float kInterpolationRate = 0.1f;

	//速度の掛け率
	static inline const float kVelocityBias = 0.1f;

	//追従対象の各方向へのカメラ移動範囲
	static inline const Rect margin = { -5.0f,5.0f,-5.0f,5.0f };
private://メンバ変数
	//カメラ
	Camera* camera_ = nullptr;
	//追従対象
	Player* target_ = nullptr;
	//追従対象とカメラの座標の差(オフセット)
	Vector3 targetOffset_ = { 0.0f,0.0f,-15.0f };
	//移動範囲
	Rect movableArea_ = { 0.0f,100.0f,0.0f,100.0f };
	//目的地
	Vector3 goalPosition_ = {};
};

