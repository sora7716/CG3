#include "TextRasterizer.h"
#include <cassert>

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"windowscodecs.lib")

using Microsoft::WRL::ComPtr;

//初期化
void TextRasterizer::Initialize() {
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic_));
	assert(SUCCEEDED(hr));

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&d2dFactory_));
	assert(SUCCEEDED(hr));

	hr= DWrite	
}
