#pragma once
#include "engine/math/ResourceData.h"
#include "engine/math/RenderingData.h"

//前方宣言
class Object3d;
class Camera;

/// <summary>
/// スポットライト
/// </summary>
class SpotLight{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SpotLight() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SpotLight() = default;

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
	/// デバッグ用
	/// </summary>
	void Debug();

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
	/// スポットライトデータのゲッター
	/// </summary>
	/// <returns>スポットライト</returns>
	SpotLightData& GetSpotLightData();
private://メンバ変数
	//3dオブジェクト
	Object3d* object3d_ = nullptr;

	//スポットライト
	SpotLightData spotLightData_ = {};

	//マテリアル
	Material material_ = {};
};

