#pragma once
#include "IScene.h"

//前方宣言
class Input;

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
	void Initialize(DirectXBase* directXBase)override;

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

public://静的メンバ変数
	static inline const float kLookRadPerCount = 1.0f / 1000.0f;

private://メンバ変数
	//3Dモデル
	std::unique_ptr<Object3d>object3d_ = nullptr;
	TransformData transformData3d_ = { {1.0f,1.0f,1.0f} };
	//デバッグカメラ
	Camera* debugCamera_ = nullptr;
	Vector3 cameraRotate_ = {};
	Vector3 cameraTranslate_ = {};
	//入力
	Input* input_ = nullptr;
	Vector3 cameraMoveDir = {};
	Vector2 cameraFlickVector_ = {};
	float cameraSpeed_ = 0.1f;
};
