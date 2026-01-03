#pragma once
#include "engine/math/Vector3.h"
#include "engine/math/Matrix4x4.h"
#include "engine/math/ResourceData.h"
#include <string>
#include <vector>

// 前方宣言
class Camera;
class Object3d;
class WorldTransform;

/// <summary>
/// レールカメラ
/// </summary>
class RailCamera {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RailCamera() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RailCamera() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="rotate">回転</param>
	/// <param name="position">位置</param>
	void Initialize(Camera* camera, const Vector3& rotate, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// ワールドトランスフォームのゲッター
	/// </summary>
	/// <returns>ワールドトランスフォーム</returns>
	const WorldTransform* GetWorldTransform()const;

	/// <summary>
	/// カメラのゲッター
	/// </summary>
	/// <returns>カメラ</returns>
	Camera* GetCamera();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// 制御ポイントのセッター
	/// </summary>
	/// <param name="controlPoints">制御ポイント</param>
	void SetControlPoints(std::vector<Vector3>controlPoints);
private://メンバ変数
	//カメラ
	Camera* camera_ = nullptr;

	//カメラのオブジェクト
	Object3d* cameraObject_ = nullptr;

	//回転
	Vector3 rotate_ = { 0.0f,0.0f,0.0f };
	//平行移動
	Vector3 translate_ = { 0.0f,0.0f,0.0f };
	//マテリアル
	Material material_ = {};
	//制御ポイント
	std::vector<Vector3>controlPoints_;

	//フレーム数
	float frame_ = 0.0f;
	float endFrame_ = 120.0f;

	//レールカメラスタートフラグ
	bool isMovingCamera_ = false;

	//調整項目のグループ名
	std::string groupName_ = "railCamera";
};

