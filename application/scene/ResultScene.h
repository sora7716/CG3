#pragma once
#include "engine/scene/IScene.h"

/// <summary>
/// タイトルシーン
/// </summary>
class ResultScene :public IScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ResultScene() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ResultScene()override = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="input">input</param>
	/// <param name="cameraManager">カメラマネージャー</param>
	void Initialize(DirectXBase* directXBase, Input* input, CameraManager* cameraManager)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://メンバ変数
	Camera* camera_ = nullptr;
	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
};
