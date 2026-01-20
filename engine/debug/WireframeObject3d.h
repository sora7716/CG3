#pragma once
#include "engine/3d/Model.h"
#include "engine/math/CollisionPrimitives.h"
#include "engine/math/ResourceData.h"
#include "engine/math/RenderingData.h"
#include "engine/base/BlendMode.h"
#include "engine/worldTransform/WorldTransform.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <d3d12.h>
//前方宣言
class Camera;
class DirectXBase;
class Model;

enum class ModelType {
	kSphere,
	kCube
};

/// <summary>
/// 3Dオブジェクト
/// </summary>
class WireframeObject3d{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WireframeObject3d() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~WireframeObject3d();

	/// <summary>
    /// 初期化
    /// </summary>
    /// <param name="camera">カメラ</param>
	/// <param name="modelType">モデルタイプ</param>
	void Initialize(Camera* camera,ModelType modelType);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 親子付け
	/// </summary>
	/// <param name="parent">親</param>
	void Compose(const WorldTransform* parent);

	/// <summary>
	/// 親子関係を解除
	/// </summary>
	void Decompose();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// 半径のセッター
	/// </summary>
	/// <param name="radius">半径</param>
	void SetRadius(float radius);

	/// <summary>
	/// スケールのセッター
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetScale(const Vector3& scale);

	/// <summary>
	/// 回転のセッター
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetRotate(const Vector3& rotate);

	/// <summary>
	/// 平行移動のセッター
	/// </summary>
	/// <param name="translate">平行移動</param>
	void SetTranslate(const Vector3& translate);

	/// <summary>
	/// 色のセッター
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Vector4& color);

	/// <summary>
	/// 親のセッター
	/// </summary>
	/// <param name="parent">親</param>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// ブレンドモードのセッター
	/// </summary>
	/// <param name="blendMode"></param>
	void SetBlendMode(const BlendMode& blendMode);

	/// <summary>
	/// スケールのゲッター
	/// </summary>
	/// <returns>スケール</returns>
	float GetRadius()const;

	/// <summary>
	/// スケールのゲッター
	/// </summary>
	/// <returns></returns>
	Vector3 GetScale()const;

	/// <summary>
	/// 回転のゲッター
	/// </summary>
	/// <returns>回転</returns>
	Vector3 GetRotate()const;

	/// <summary>
	/// 平行移動のゲッター
	/// </summary>
	/// <returns>平行移動</returns>
	Vector3 GetTranslate()const;

	/// <summary>
	/// 色のゲッター
	/// </summary>
	/// <returns>色</returns>
	Vector4 GetColor()const;

	/// <summary>
	/// ワールドトランスフォームのゲッター
	/// </summary>
	/// <returns>ワールドトランスフォーム</returns>
	WorldTransform* GetWorldTransform()const;

	/// <summary>
	/// ワールド座標のゲッター
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos();

	/// <summary>
	/// 球のゲッター
	/// </summary>
	/// <returns>球</returns>
	Sphere GetSphere()const;

	/// <summary>
	/// AABBのゲッター
	/// </summary>
	/// <returns>AABB</returns>
	AABB GetAABB()const;

	/// <summary>
	/// OBBのゲッター
	/// </summary>
	/// <returns></returns>
	OBB GetOBB()const;
private://メンバ変数
	//UV座標
	Transform2dData uvTransform_ = {
		.scale = { 1.0f,1.0f },
		.rotate = 0.0f,
		.translate = {0.0f,0.0f}
	};
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//3Dモデル
	Model* model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform* worldTransform_ = nullptr;
	float radius_ = 1.0f;
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;

	//マテリアル
	Material material_ = {};

	//衝突判定に使用するプリミティブのデータ
	Sphere sphere_ = {};//球
	AABB aabb_ = {};//AABB
	OBB obb_ = {};//OBB
};

