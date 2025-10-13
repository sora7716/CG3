#pragma once
#include "engine/math/Vector3.h"
#include <string>

//前方宣言
class Camera;
class Player;

/// <summary>
/// カメラコントローラ
/// </summary>
class CameraController{
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
	/// <param name="cameraName">カメラ名</param>
	void Initialize(const std::string& cameraName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
    /// 追従対象のセッター
    /// </summary>
	/// <param name="target">追従対象</param>
	void SetTarget(Player*target);

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();
private://メンバ変数
	//カメラ
	Camera* camera_ = nullptr;
	//追従対象
	Player* target_ = nullptr;
	//追従対象とカメラの座標の差(オフセット)
	Vector3 targetOffset_ = { 0.0f,0.0f,-15.0f };
};

