#pragma once
#include "WinApi.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "engine/input/Input.h"
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
	WinApi* GetWinApi()const;

	/// <summary>
	/// DirectXの基盤部分
	/// </summary>
	/// <returns>DirectXの基盤部分</returns>
	DirectXBase* GetDirectXBase()const;

	/// <summary>
	/// SRVマネージャーのゲッター
	/// </summary>
	/// <returns>SRVマネージャー</returns>
	SRVManager* GetSRVManager()const;

	/// <summary>
	/// 入力のゲッター
	/// </summary>
	/// <returns>入力</returns>
	Input* GetInput()const;

	/// <summary>
	/// カメラマネージャーのゲッター
	/// </summary>
	/// <returns>カメラマネージャー</returns>
	CameraManager* GetCameraManager()const;
private://メンバ変数
	//WinApi
	std::unique_ptr<WinApi>winApi_ = nullptr;
	//DirectXの基盤部分
	std::unique_ptr<DirectXBase>directXBase_ = nullptr;
	//SRVマネージャー
	std::unique_ptr<SRVManager>srvManager_ = nullptr;
	//入力
	std::unique_ptr<Input>input_ = nullptr;
	//カメラマネージャー
	std::unique_ptr<CameraManager>cameraManager_ = nullptr;
};

