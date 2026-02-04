#pragma once
#include "engine/math/ResourceData.h"
#include "engine/math/RenderingData.h"
#include "engine/base/BlendMode.h"
#include "TextRasterizer.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>

//前方宣言
class WorldTransform;
class Camera;
class DirectXBase;

//テキストの生成時に必要なもの
//text:入力する文字
//font:使用するフォント
//size:テキストのサイズ
//color:テキストの色
struct TextStyle {
	std::string text;
	std::string font;
	float size;
	Vector4 color;
};

/// <summary>
/// 文字
/// </summary>
class Text{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Text() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Text();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textKey">テキストを検索するとき用のキー</param>
	void Initialize(const std::string& textKey);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();
private://メンバ関数
	/// <summary>
	/// 頂点データの初期化
	/// </summary>
	void InitializeVertexData();

	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// インデックスリソースの生成
	/// </summary>
	void CreateIndexResource();

	/// <summary>
	/// マテリアルデータの初期化
	/// </summary>
	void InitializeMaterialData();

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// テキストのテクスチャを取得または生成
	/// </summary>
	void AcquireTextTexture();
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;
	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
	//バッファリソース内のデータを指すポインタ
	Material* materialData_ = nullptr;
	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス	

	//ワールドトランスフォーム
	WorldTransform* worldTransform_ = nullptr;
	//UV座標
	Transform2dData uvTransform_ = {
		.scale = { 1.0f,1.0f },
		.rotate = 0.0f,
		.translate = {0.0f,0.0f}
	};
	//ワールド座標
	Transform2dData transform_ = {
		.scale = {100.0f,100.0f},
		.rotate = 0.0f,
		.translate = {0.0f,0.0f}
	};

	//テキストデータ
	TextData textData_ = {};
	//テキストスタイル
	TextStyle textStyle_ = {};
	//テキストラスタライザー
	TextRasterizer* textRasterizer_ = nullptr;
};

