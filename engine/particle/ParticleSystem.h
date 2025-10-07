#pragma once
#include "engine/math/ResourceData.h"
#include "engine/math/RenderingData.h"
#include "engine/base/BlendMode.h"
#include <d3d12.h>
#include <wrl.h>
#include <random>
#include <list>

//前方宣言
class DirectXBase;
class Camera;
class Model;

struct AABB {
	Vector3 min;//最小値
	Vector3 max;//最大値
};

//構造体
//パーティクル単体のデータ
struct Particle {
	TransformData transform;//SRVの情報
	Vector3 velocity;//速度
	Vector3 direction;//方向
	Vector4 color;//色
	float lifeTime;//生存時間
	float currentTime;//発生してからの
};

//パーティクルの情報をGPUに送るための構造体
struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

//発生源
struct Emitter {
	TransformData transform;//エミッターのTransform
	uint32_t count;//発生数
	float frequency;//発生頻度
	float frequencyTime;//頻度用時刻
};

//フィールドの加速度
struct AccelerationField {
	Vector3 acceleration;//加速度
	AABB area;//範囲
};

/// <summary>
/// パーティクルシステム
/// </summary>
class ParticleSystem {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleSystem() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleSystem() = default;

	/// <summary>
    /// 初期化
    /// </summary>
    /// <param name="directXBase">DirectXの基盤部分</param>
    /// <param name="textureName">テクスチャ名</param>
	/// <param name="model">モデル</param>
	void Initialize(DirectXBase* directXBase, const std::string& textureName, Model* model = nullptr);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// テクスチャ名のゲッター
	/// </summary>
	/// <returns>テクスチャ名</returns>
	std::string GetTextureName();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);
private://メンバ関数
	/// <summary>
	/// モデルデータの初期化
	/// </summary>
	void InitializeQuadModelData();

	/// <summary>
	/// マテリアルデータの初期化
	/// </summary>
	void InitializeMaterialData();

	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// インデックスリソースの生成
	/// </summary>
	void CreateIndexResource();

	/// <summary>
	/// ワールドトランスフォームのリソースの生成
	/// </summary>
	void CreateWorldTransformResource();

	/// <summary>
	/// ワールドトランスフォームの更新
	/// </summary>
	/// <param name="numInstance">インスタンス数</param>
	/// <param name="iterator"イテレータ></param>
	void UpdateWorldTransform(uint32_t numInstance, auto iterator);

	/// <summary>
	/// ストラクチャバッファの生成
	/// </summary>
	void CreateStructuredBuffer();

	/// <summary>
	/// パーティクルの生成
	/// </summary>
	/// <returns>新しいパーティクル</returns>
	Particle MakeNewParticle();

	/// <summary>
	/// パーティクルの発生
	/// </summary>
	/// <returns>パーティクル</returns>
	std::list<Particle>Emit();

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="aabb">AABB</param>
	/// <param name="point">point</param>
	/// <returns>衝突判定</returns>
	bool IsCollision(const AABB& aabb, const Vector3& point);

	/// <summary>
	/// 矩形状に移動させる
	/// </summary>
	/// <param name="translate">平行移動</param>
	/// <param name="direction">移動する向き</param>
	/// <param name="velocity">速度</param>
	void EmitOnRect(Vector3& translate,const Vector3& direction,const Vector3& velocity);
private://静的メンバ変数
	//パーティクルの数
	static const uint32_t kNumMaxInstance = 1024;
private://メンバ変数
	//DirectXの基盤部分	
	DirectXBase* directXBase_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;
	//ワールドビュープロジェクションのリソース
	ComPtr<ID3D12Resource>instancingResource_ = nullptr;
	//ワールドビュープロジェクションのデータ
	ParticleForGPU* instancingData_ = {};
	//モデル
	Model* model_ = nullptr;
	//モデルデータ
	ModelData modelData_ = {};
	//マテリアルデータ
	Material* materialData_ = nullptr;
	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス	
	//SRVインデックス
	uint32_t srvIndex_ = 0;
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kAdd;
	//ランダムエンジン
	std::mt19937 randomEngine_;
	//パーティクルのデータ
	std::list<Particle> particles_ = {};
	//ワールドマトリックス
	Matrix4x4 worldMatrix_ = {};
	//生存しているパーティクルの数
	uint32_t numInstance_ = 0;
	//発生源
	Emitter emitter_ = {
		.transform = {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}},
		.count = 1	,
		.frequency = 0.1f,//発生頻度
		.frequencyTime = 0.0f//発生頻度用の時刻,0.0fで初期化
	};
	//フィールドの加速度
	AccelerationField accelerationField_ = {
		.acceleration = {15.0f,0.0f,0.0f},
		.area = {{-1.0f,-1.0f,-1.0f},1.0f,1.0f,1.0f}
	};
};
