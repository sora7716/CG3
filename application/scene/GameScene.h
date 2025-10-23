#pragma once
#include "engine/scene/IScene.h"
#include "actor/Player.h"
#include "actor/CameraController.h"
#include "actor/Field.h"
#include "actor/MapChip.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene :public IScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene()override = default;

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

private://メンバ変数
	//ゲームシーンのカメラ
	Camera* camera_ = nullptr;

	//プレイヤー
	std::unique_ptr<Player>player_ = nullptr;

	//追従カメラ
	std::unique_ptr<CameraController>cameraController_ = nullptr;

	//フィールド
	std::unique_ptr<Field>field_ = nullptr;

	//フィールド
	std::unique_ptr<Object3d>object3d_ = nullptr;
	TransformData transformData_ = { {1.0f,1.0f,1.0f},{},{} };
	Material material_ = {};

};

