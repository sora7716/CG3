#pragma once
#include "engine/3d/Model.h"
#include "engine/math/ResourceData.h"
#include "engine/math/RenderingData.h"
#include "engine/base/BlendMode.h"
#include "actor/MapChipData.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <array>
#include <d3d12.h>
//前方宣言
class Camera;
class DirectXBase;
class GraphicsPipeline;
class Blend;

/// <summary>
/// マップチップ
/// </summary>
class MapChip {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	MapChip() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MapChip() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="camera">カメラ</param>
	/// <param name="mapChipType">マップチップのタイプ</param>
	/// <param name="mapName">マップ名</param>
	/// <param name="modelName">使うモデル名</param>
	/// <param name="posY">描画開始位置のY座標</param>
	/// <param name="mapSize">マップサイズ</param>
	void Initialize(DirectXBase* directXBase, Camera* camera, MapChipType mapChipType, const std::string& mapName, const std::string& modelName, float posY, const Vector2Int& mapSize);

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
	/// マテリアルのゲッター
	/// </summary>
	/// <returns>マテリアル</returns>
	Material GetMaterial();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// ブレンドモードのセッター
	/// </summary>
	/// <param name="blendMode"></param>
	void SetBlendMode(const BlendMode& blendMode);

	/// <summary>
	/// カメラの位置のセッター
	/// </summary>
	/// <param name="cameraTranslate"></param>
	void SetCameraForGPU(const Vector3& cameraTranslate);

	/// <summary>
	/// 平行光源のセッター
	/// </summary>
	/// <param name="directionalLight">平行光源</param>
	void SetDirectionalLight(const DirectionalLight& directionalLight);

	/// <summary>
	/// 点光源のセッター
	/// </summary>
	/// <param name="pointLight">点光源</param>
	void SetPontLight(const PointLight* pointLight);

	/// <summary>
	/// スポットライトのセッター
	/// </summary>
	/// <param name="spotLight">スポットライト</param>
	void SetSpotLight(const SpotLight* spotLight);

	/// <summary>
	/// テクスチャのセッター
	/// </summary>
	/// <param name="textureName">テクスチャ名</param>
	void SetTexture(const std::string& textureName);

	/// <summary>
	/// マテリアルのセッター
	/// </summary>
	/// <param name="material">マテリアル</param>
	void SetMaterial(const Material& material);
private://メンバ関数
	/// <summary>
	/// マップチップの読み込み(csv)
	/// </summary>
	/// <param name="fileName">ファイル名(ディレクトリパスはいらない)</param>
	void LoadMapCsv(const std::string& fileName);

	/// <summary>
	/// カメラリソースの生成
	/// </summary>
	void CreateCameraResource(const Vector3& cameraTranslate);

	/// <summary>
	/// 座標変換行列リソースの生成
	/// </summary>
	void CreateTransformationMatrixResource();

	/// <summary>
	/// 座標変換行列リソースのストラクチャバッファの生成
	/// </summary>
	void CreateStructuredBufferForWvp();

	/// <summary>
	/// 座標の更新
	/// </summary>
	void UpdateTransform();

	/// <summary>
	/// uv変換
	/// </summary>
	void UVUpdateTransform();

	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// インデックスリソースの生成
	/// </summary>
	void CreateIndexResource();

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// 平行光源の生成
	/// </summary>
	void CreateDirectionLight();

	/// <summary>
	/// 点光源の生成
	/// </summary>
	void CreatePointLight();

	/// <summary>
	/// 点光源のストラクチャバッファの生成
	/// </summary>
	void CreateStructuredBufferForPoint();

	/// <summary>
	/// スポットライトの生成
	/// </summary>
	void CreateSpotLight();

	/// <summary>
	/// スポットライトのストラクチャバッファの生成
	/// </summary>
	void CreateStructuredBufferForSpot();
private://定数
	//ライトの最大値
	static inline const int32_t kMaxLightCount = 64;
	//ブロックの際打数
	static inline const int32_t kMaxBlockCount = 1024;
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	//Objファイルデータ
	ModelData modelData_ = {};

	//リソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//VertexResource
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//MaterialResource
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//IndexResource
	ComPtr<ID3D12Resource>wvpResource_ = nullptr;//wvpResource
	ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;//平行光源
	ComPtr<ID3D12Resource> pointLightResource_ = nullptr;//点光源
	ComPtr<ID3D12Resource> spotLightResource_ = nullptr;//スポットライト
	ComPtr<ID3D12Resource> cameraResource_ = nullptr;//カメラ

	//ポインタ
	VertexData* vertexPtr_ = nullptr;//Vertex
	Material* materialPtr_ = nullptr;//Material
	uint32_t* indexPtr_ = nullptr;//Index
	TransformationMatrix* wvpPtr_ = nullptr;//TransformationMatrix
	DirectionalLight* directionalLightPtr_ = nullptr;//平行光源
	PointLight* pointLightPtr_ = nullptr;//点光源
	SpotLight* spotLightPtr_ = nullptr;//スポットライト
	CameraForGPU* cameraForGPU_ = nullptr;//カメラ

	//SRVインデックス
	uint32_t srvIndexPoint_ = 0;//PointLight
	uint32_t srvIndexSpot_ = 0;//SpotLight
	uint32_t srvIndexWvp_ = 0;//Transformation

	//VertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	//IndexBufferView
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス	

	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;

	//グラフィックスパイプライン(PSO)
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates_ = { nullptr };
	//グラフィックスパイプライン
	GraphicsPipeline* makeGraphicsPipeline_ = nullptr;

	//平行光源
	DirectionalLight directionalLightData_ = {};
	//点光源
	PointLight pointLightDataList_[kMaxLightCount] = {};
	//スポットライト
	SpotLight spotLightDataList_[kMaxLightCount] = {};

	//ワールド座標
	TransformData transforms_[kMaxBlockCount] = {};

	//ワールド行列
	TransformationMatrix wvpDataList_[kMaxBlockCount] = {};

	//ブレンド
	Blend* blend_ = nullptr;
	BlendMode blendMode_ = BlendMode::kNone;

	//UV座標
	Transform2dData uvTransform_ = {
		.scale = { 1.0f,1.0f },
		.rotate = 0.0f,
		.translate = {0.0f,0.0f}
	};

	//マテリアル
	Material materialData_ = {};

	//マップのサイズ
	Vector2Int mapSize_ = { 0,0 };
	//生成されるブロック数
	uint32_t generatedBlockCount_ = 0;

	//マップ
	MapChipData mapChipData_;
};
