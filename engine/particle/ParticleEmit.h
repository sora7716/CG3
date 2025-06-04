#pragma once
#include "engine/math/ResourceData.h"
#include "engine/math/RenderingData.h"
#include "engine/blend/BlendMode.h"
#include <d3d12.h>
#include <wrl.h>
#include <random>
#include <list>

//前方宣言
class DirectXBase;
class Camera;

//構造体
//パーティクル単体のデータ
typedef struct Particle {
	Transform transform;//SRVの情報
	Vector3 velocity;//速度
	Vector4 color;//色
	float lifeTime;//生存時間
	float currentTime;//発生してからの
}Particle;

//パーティクルの情報をGPUに送るための構造体
typedef struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
}ParticleForGPU;

/// <summary>
/// パーティクルのエミット
/// </summary>
class ParticleEmit {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleEmit() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleEmit() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	void Initialize(DirectXBase* directXBase);

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
private://メンバ関数
	/// <summary>
	/// モデルデータの初期化
	/// </summary>
	void InitializeModelData();

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
	/// ワールドトランスフォームのリソースの生成
	/// </summary>
	void CreateWorldTransformResource();

	/// <summary>
	/// ワールドトランスフォームの更新
	/// </summary>
	void UpdateWorldTransform();

	/// <summary>
	/// ストラクチャバッファの生成
	/// </summary>
	void CreateStructuredBuffer();

	/// <summary>
	/// パーティクルの生成
	/// </summary>
	/// <param name="randomEngine">ランダムエンジン</param>
	/// <returns>新しいパーティクル</returns>
	Particle MakeNewParticle(std::mt19937& randomEngine);
private://静的メンバ変数
	//パーティクルの数
	static const uint32_t kNumMaxInstance = 3;
private://メンバ変数
	//DirectXの基盤部分	
	DirectXBase* directXBase_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;
	//ワールドビュープロジェクションのリソース
	ComPtr<ID3D12Resource>instancingResource_ = nullptr;
	//ワールドビュープロジェクションのデータ
	ParticleForGPU* instancingData_ = {};
	//パーティクルのデータ
	std::list<Particle> particles_ = {};
	//ワールドマトリックス
	Matrix4x4 worldMatrix_ = {};
	//モデルデータ
	ModelData modelData_ = {};
	//マテリアルデータ
	Material* materialData_ = nullptr;
	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kAdd;
	//SRVインデックス
	uint32_t srvIndex_ = 0;
	//生存しているパーティクルの数
	uint32_t numInstance_ = 0;
	//ランダムエンジン
	std::mt19937 randomEngine_;
};
