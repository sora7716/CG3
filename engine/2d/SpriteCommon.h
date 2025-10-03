#pragma once
#include "engine/math/ResourceData.h"
#include "engine/base/blendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <array>
#include <string>
//前方宣言
class DirectXBase;
class Blend;
class GraphicsPipeline;

/// <summary>
/// スプライトの共通部分
/// </summary>
class SpriteCommon {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// インスタンスのゲッター
	/// </summary>
	/// <returns></returns>
	static SpriteCommon* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	void Initialize(DirectXBase* directXBase);

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawSetting();

	/// <summary>
	/// テクスチャの読み込み
	/// </summary>
	/// <param name="filename">ファイル名</param>
	void LoadTexture(const std::string& filename);

	/// <summary>
	/// 光源の生成
	/// </summary>
	void CreateDirectionLight();

	/// <summary>
	/// DirectionalLightのセッター
	/// </summary>
	/// <param name="directionalLightData">DirectionalLightデータ</param>
	void SetDirectionalLightData(const DirectionalLight& directionalLightData);

	/// <summary>
	/// DirectionalLightのリソースのゲッター
	/// </summary>
	/// <returns>DirectionalLightのリソース</returns>
	ID3D12Resource* GetDirectionalLightResource()const;

	/// <summary>
	/// DirectXの基盤部分のゲッター
	/// </summary>
	/// <returns></returns>
	DirectXBase* GetDirectXBase();

	/// <summary>
	/// グラフィックパイプラインのゲッター
	/// </summary>
	/// <returns>グラフィックパイプライン</returns>
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)>GetGraphicsPipelineStates()const;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();
private://メンバ関数
	//コンストラクタを封印
	SpriteCommon() = default;
	//デストラクタを封印
	~SpriteCommon() = default;
	//コピーコンストラクタ禁止
	SpriteCommon(const SpriteCommon&) = delete;
	//代入演算子の禁止
	SpriteCommon operator=(const SpriteCommon&) = delete;
private://静的メンバ変数
	//インスタンス
	static inline SpriteCommon* instance = nullptr;
	//Finalizeしたかどうかのフラグ
	static inline bool isFinalize = false;
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;
	//グラフィックスパイプライン(PSO)
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates_ = { nullptr };
	//グラフィックスパイプライン
	GraphicsPipeline* makeGraphicsPipeline_ = nullptr;
	//バッファリソース
	ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;//光源
	//バッファリソース内のデータを指すポインタ
	DirectionalLight* directionalLightData_ = nullptr;//光源
	//ブレンド
	Blend* blend_ = nullptr;
};