#pragma once
#include "ActorData.h"
#include <string>

// 前方宣言
class Object3d;
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
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグテキスト
	/// </summary>
	void DebugText();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="cameraName">カメラ名</param>
	void SetCamera(const std::string& cameraName);

	/// <summary>
	/// トランスフォームデータのゲッター
	/// </summary>
	/// <returns>トランスフォームデータ</returns>
	TransformData GetTransformData();
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
};

