#pragma once
#include "ActorData.h"
#include <string>
#include <vector>

//カメラ
class Object3dCommon;
class Object3d;
class Camera;

/// <summary>
/// フィールド
/// </summary>
class Field {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Field();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Field();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">Object3dの共通部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(Object3dCommon*object3dCommon,Camera*camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 衝突したとき
	/// </summary>
	/// <param name="colliderState">コライダーの状態</param>
	void OnCollision(ColliderState* colliderState);

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera*camera);
private://メンバ変数
	//ゲームオブジェクト
	GameObject gameObject_{};
	//レンダーオブジェクト
	RenderObject renderObject_ = {};
	//モデル名
	std::string modelName_ = "wall";
	//コライダー
	ColliderState colliderState_ = {};
	Collider collider_ = {};
};


