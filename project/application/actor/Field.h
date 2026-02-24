#pragma once
#include "engine/math/RenderingData.h"
#include "engine/math/ResourceData.h"
#include "MapChipData.h"
#include <string>
#include <vector>

//カメラ
class Object3dCommon;
class Object3d;
class Camera;
class MapChip;

/// <summary>
/// フィールド
/// </summary>
class Field {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Field() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Field();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">3Dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(Object3dCommon*object3dCommon,Camera* camera);

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
	/// 調整項目を適応
	/// </summary>
	void ApplyGlobalVariables();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// テクスチャのセッター
	/// </summary>
	/// <param name="textureName">テクスチャ名</param>
	void SetTexture(const std::string& textureName);

	/// <summary>
	/// 平行光源のセッター
	/// </summary>
	/// <param name="directional">平行光源</param>
	void SetDirectionalLight(const DirectionalLight& directional);

	/// <summary>
	/// 点光源のセッター
	/// </summary>
	/// <param name="point">点光源</param>
	void SetPointLight(const PointLight* point);

	/// <summary>
	/// スポットライトのセッター
	/// </summary>
	/// <param name="spot">スポットライト</param>
	void SetSpotLight(const SpotLight* spot);
private://メンバ変数
	//マップチップ
	MapChip* mapChip_ = nullptr;
	//マップのサイズ
	Vector2Int mapSize_ = { 20,20 };
	//調整項目のグループ名
	const char* groundGroupName_ = "ground";
};


