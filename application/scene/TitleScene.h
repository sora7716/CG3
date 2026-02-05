#pragma once
#include "engine/scene/IScene.h"
#include "engine/2d/TextRasterizer.h"
#include "engine/2d/Text.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene :public IScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TitleScene() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene()override = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="cameraManager">カメラマネージャー</param>
	void Initialize(DirectXBase* directXBase, CameraManager* cameraManager)override;

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

	std::unique_ptr<Text>textObj_ = nullptr;
	TextStyle textStyele_ = {};
	Transform2dData textTransformData_ = {};
};
