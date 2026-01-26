#pragma once
#include <wrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <cstdint>
#include <vector>
#include <string>

//CPUビットマップ
struct CpuBitmap {
	uint32_t width;
	uint32_t height;
	uint32_t stride;//1行のバイト数
	std::vector<uint8_t>bgra;//BGRA 4byte/pixel
};

/// <summary>
/// 文字のラスタライザ
/// </summary>
class TextRasterizer{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TextRasterizer() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TextRasterizer() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
private://メンバ変数
	ComPtr<IWICImagingFactory>wic_;
	ComPtr<ID2D1Factory>d2dFactory_;
	ComPtr<IDWriteFactory>dwrite_;
};

