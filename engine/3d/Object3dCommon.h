#pragma once
#include "engine/math/ResourceData.h"
#include "engine/base/blendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <array>
#include <map>

//前方宣言
class DirectXBase;
class Camera;
class GraphicsPipeline;
class Blend;

/// <summary>
/// 3Dオブジェクトの共通部分
/// </summary>
class Object3dCommon {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// インスタンスのゲッター
	/// </summary>
	/// <returns></returns>
	static Object3dCommon* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤</param>
	/// <param name="directionalLightData">directionalLightのデータ</param>
	void Initialize(DirectXBase* directXBase);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawSetting();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// カメラリソースの生成
	/// </summary>
	void CreateCameraResource(const Vector3& cameraTranslate);

	/// <summary>
	/// カメラの位置のセッター
	/// </summary>
	/// <param name="cameraTranslate"></param>
	void SetCameraForGPU(const Vector3& cameraTranslate);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// DirectionalLightのリソースのゲッター
	/// </summary>
	/// <returns>DirectionalLightのリソース</returns>
	ID3D12Resource* GetDirectionalLightResource()const;

	/// <summary>
	/// PointLightのリソースのゲッター
	/// </summary>
	/// <returns>PointLightのリソース</returns>
	ID3D12Resource* GetPointLightResource()const;

	/// <summary>
	/// SpotLightのリソースのゲッター
	/// </summary>
	/// <returns>SpotLightのリソース</returns>
	ID3D12Resource* GetSpotLightResource()const;

	/// <summary>
	/// DirectXの基盤のゲッター
	/// </summary>
	/// <returns>DirectXの基盤</returns>
	DirectXBase* GetDirectXBase()const;

	/// <summary>
	/// グラフィックパイプラインのゲッター
	/// </summary>
	/// <returns>グラフィックパイプライン</returns>
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)>GetGraphicsPipelineStates()const;

	/// <summary>
	/// デフォルトカメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetDefaultCamera(Camera* camera);

	/// <summary>
	/// デフォルトカメラのゲッター
	/// </summary>
	/// <returns>デフォルトカメラ</returns>
	Camera* GetDefaultCamera()const;

	/// <summary>
	/// SRVインデックスのゲッター(PointLight)
	/// </summary>
	/// <returns>SRVインデックス</returns>
	uint32_t GetSrvIndexPoint()const;

	/// <summary>
	/// SRVインデックスのゲッター(SpotLight)
	/// </summary>
	/// <returns>SRVインデックス</returns>
	uint32_t GetSrvIndexSpot()const;

	/// <summary>
	/// 平行光源のゲッター
	/// </summary>
	/// <returns>平行光源</returns>
	const DirectionalLight& GetDirectionalLight()const;

	/// <summary>
	/// 点光源のセッター
	/// </summary>
	/// <returns>点光源</returns>
	PointLight* GetPointLight();

	/// <summary>
	/// スポットライトのゲッター
	/// </summary>
	/// <returns>スポットライト</returns>
	SpotLightData* GetSpotLightPtr();

	/// <summary>
	/// スポットライトを追加
	/// </summary>
	/// <param name="name">スポットライトの名前</param>
	void AddSpotLight(const std::string& name);

	/// <summary>
	/// スポットライトのセッター
	/// </summary>
	/// <param name="name">スポットライトの名前</param>
	void SetSpotLight(const std::string& name,const SpotLightData&spotLight);

	/// <summary>
	/// スポットライトのゲッター
	/// </summary>
	/// <param name="name">スポットライトの名前</param>
	/// <returns>スポットライト</returns>
	SpotLightData& GetSpotLight(const std::string& name);
private://メンバ関数
	//コンストラクタの封印
	Object3dCommon() = default;
	//デストラクタの封印
	~Object3dCommon() = default;
	//コピーコンストラクタ禁止
	Object3dCommon(const Object3dCommon&) = delete;
	//代入演算子の禁止
	Object3dCommon operator=(const Object3dCommon&) = delete;

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

	/// <summary>
	/// グローバル変数に追加(PointLight)
	/// </summary>
	/// <param name="groupName">グループ名</param>
	/// <param name="pointLight">スポットライト</param>
	void AddItemForPointLight(const char* groupName, const PointLight& pointLight);

	/// <summary>
	/// グローバル変数に追加(SpotLight)
	/// </summary>
	/// <param name="groupName">グループ名</param>
	/// <param name="spotLight">スポットライト</param>
	void AddItemForSpotLight(const char* groupName, const SpotLightData& spotLight);

	/// <summary>
	/// グローバル変数を適用(PointLight)
	/// </summary>
	/// <param name="groupName">グループ名</param>
	/// <param name="pointLight">ポイントライト</param>
	void ApplyGlobalVariablesForPointLight(const char* groupName, PointLight& pointLight);

	/// <summary>
	/// グローバル変数を適応(SpotLight)
	/// </summary>
	/// <param name="groupName">グループ名</param>
	/// <param name="spotLight">スポットライト</param>
	void ApplyGlobalVariablesForSpotLight(const char* groupName, SpotLightData& spotLight);
private://静的メンバ変数
	//インスタンス
	static inline Object3dCommon* instance = nullptr;
	//Finalizeをしたかどうかのフラグ
	static inline bool isFinalize = false;
	//ライトの最大値
	static inline const int32_t kMaxLightCount = 64;
private://メンバ変数
	//DirectXの基盤
	DirectXBase* directXBase_ = nullptr;

	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;

	//グラフィックスパイプライン(PSO)
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates_ = { nullptr };
	//グラフィックスパイプライン
	GraphicsPipeline* makeGraphicsPipeline_ = nullptr;

	//バッファリソース
	ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;//平行光源
	ComPtr<ID3D12Resource> pointLightResource_ = nullptr;//点光源
	ComPtr<ID3D12Resource> spotLightResource_ = nullptr;//スポットライト
	ComPtr<ID3D12Resource> cameraResource_ = nullptr;//カメラ
	//バッファリソース内のデータを指すポインタ
	DirectionalLight* directionalLightPtr_ = nullptr;//平行光源
	PointLight* pointLightPtr_ = nullptr;//点光源
	SpotLightData* spotLightPtr_ = nullptr;//スポットライト
	CameraForGPU* cameraForGPU_ = nullptr;//カメラ

	//平行光源
	DirectionalLight directionalLightData_ = {};
	//点光源
	PointLight pointLightDataList_[kMaxLightCount] = {};
	//スポットライト
	std::map<std::string, SpotLightData>spotLightDataList_ = {};

	//ブレンド
	Blend* blend_ = nullptr;
	BlendMode blendMode_ = BlendMode::kNone;

	//デフォルトカメラ
	Camera* defaultCamera_ = nullptr;

	//SRVインデックス
	uint32_t srvIndexPoint_ = 0;//PointLight
	uint32_t srvIndexSpot_ = 0;//SpotLight

	//ライトのグループ名
	std::string pointLightGroupNames_[kMaxLightCount] = { "pointLightGroup" };
	std::string spotLightGroupNames_[kMaxLightCount] = { "spotLightGroup" };
};
