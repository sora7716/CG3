#pragma once
#include "engine/math/Vector3.h"
#include "engine/math/Vector4.h"
#include <cstdint>
#include <vector>

//前方宣言
class Object3d;
class Camera;

//制御ポイント
struct ControlPointData {
	Object3d* object3d;
	Vector3 point;
	Vector3 scale;
	Vector4 color;
};
/// <summary>
/// 制御ポイント
/// </summary>
class ControlPoint{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ControlPoint() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ControlPoint() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Initialize(Camera*camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// グローバル変数に適応
	/// </summary>
	void ApplyGlobalVariables();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
    /// カメラのセッター
    /// </summary>
    /// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// 制御ポイントのゲッター
	/// </summary>
	/// <returns>制御ポイント</returns>
	std::vector<Vector3>GetControlPoints();
private://定数
	//制御ポイントの数
	static inline const int32_t kMaxPointCount = 5;
private://メンバ変数
	//カメラ
	Camera* camera_ = nullptr;
	//制御ポイント
	std::vector<ControlPointData> controlPointDataList_ = {};
	//グループ名
	const char* groupName_ = "controlPoint";
};

