#pragma once
#include "ActorData.h"
#include <string>
#include <vector>

//カメラ
class Object3dCommon;
class Object3d;
class Camera;

//壁に必要な情報
struct WallDesc {
	GameObject gameObject;
	Vector3 hitBoxScale;
	ColliderState colliderState;
	Collider collider;
};

//壁に必要な情報のまとまり
struct WallGroup {
	std::vector<WallDesc>wallDescs;
	RenderObject renderObject;
	std::string modelName;
	int32_t wallCount;
};

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
	void Initialize(Object3dCommon* object3dCommon, Camera* camera);

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
	void SetCamera(Camera* camera);

	/// <summary>
	/// 壁に必要な情報のゲッター
	/// </summary>
	/// <returns>我部に必要な情報</returns>
	std::vector<WallDesc>& GetWallDescs();
private://メンバ変数
	//壁に必要な情報のグループ
	WallGroup wallGroup_ = {};
};


