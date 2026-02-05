#pragma once
#include "WinApi.h"
#include "DirectXBase.h"
#include "engine/camera/CameraManager.h"
#include <memory>

/// <summary>
/// エンジンの核
/// </summary>
class Core{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Core() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Core() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// WinApiのゲッター
	/// </summary>
	/// <returns>WinApi</returns>
	WinApi* GetWinApi();

	/// <summary>
	/// DirectXの基盤部分
	/// </summary>
	/// <returns>DirectXの基盤部分</returns>
	DirectXBase* GetDirectXBase();

	/// <summary>
	/// カメラマネージャーのゲッター
	/// </summary>
	/// <returns>カメラマネージャー</returns>
	CameraManager* GetCameraManager();
private://メンバ変数
	//WinApi
	std::unique_ptr<WinApi>winApi_ = nullptr;
	//DirectXの基盤部分
	std::unique_ptr<DirectXBase>directXBase_ = nullptr;
	//カメラマネージャー
	std::unique_ptr<CameraManager>cameraManager_ = nullptr;
};

