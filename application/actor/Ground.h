#pragma once
#include "engine/math/RenderingData.h"
#include "engine/math/ResourceData.h"
#include <string>
#include <vector>

//カメラ
class Camera;
class Object3d;

//地面に必要なデータ
struct GroundData {
	Object3d* object;
	TransformData transform;
};

/// <summary>
/// 地面
/// </summary>
class Ground {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Ground() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Ground() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="modelName">モデル名</param>
	void Initialize(Camera* camera, const std::string& modelName);

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
	/// デバッグ
	/// </summary>
	void Debug();

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
private://定数
	//ブロックの数
	static inline const Vector2Int kBlockCount = { 50,50 };
private://メンバ変数
	//地面のデータ
	std::vector<std::vector<GroundData>>groundDates_;
	//マテリアル
	Material material_ = {};
};


