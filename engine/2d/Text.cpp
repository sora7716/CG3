#include "Text.h"
#include "Object2dCommon.h"
#include "TextureManager.h"
#include "engine/worldTransform/WorldTransform.h"
#include "engine/base/DirectXBase.h"
#include "engine/base/WinApi.h"
#include "engine/math/func/Rendering.h"
#include "engine/math/func/Math.h"
#include "engine/camera/Camera.h"
#include "engine/base/StringUtility.h"

//デストラクタ
Text::~Text() {
	Finalize();
}

//初期化
void Text::Initialize(const std::string& textKey) {
	//DirectXの基盤部分を記録する
	directXBase_ = Object2dCommon::GetInstance()->GetDirectXBase();
	//頂点データの生成
	CreateVertexResource();
	//インデックスリソースの生成
	CreateIndexResource();
	//マテリアルデータの生成
	CreateMaterialResource();
	//テキストラスタライザーの生成と初期化
	textRasterizer_ = new TextRasterizer();
	textRasterizer_->Initialize();
	//テキストの検索キーを記録
	textData_.textKey = textKey;
	//テキストを設定
	textStyle_.text = "Sample";
	textStyle_.font = "\0";
	textStyle_.size = 32.0f;
	textStyle_.color = Vector4::MakeWhiteColor();
	//テキストのテクスチャの生成
	AcquireTextTexture();
	//テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture(textData_.textKey);
	//uvTransform変数を作る
	uvTransform_ = { {1.0f,1.0f},Math::kPi,{0.0f,0.0f} };
	//ワールドトランスフォームの生成と初期化
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize(directXBase_, TransformMode::k2d);
	//カメラにデフォルトカメラを代入
	worldTransform_->SetCamera(Object2dCommon::GetInstance()->GetDefaultCamera());
	//スクリーンに表示する範囲を設定
	WorldTransform::ScreenArea screenArea = {
		.left = -static_cast<float>(WinApi::kClientWidth / 2),
		.top = -static_cast<float>(WinApi::kClientHeight / 2),
		.right = static_cast<float>(WinApi::kClientWidth / 2),
		.bottom = static_cast<float>(WinApi::kClientHeight / 2),
	};
	worldTransform_->SetScreenArea(screenArea);
}

//更新
void Text::Update() {
	//トランスフォームのセッター
	worldTransform_->SetTransform2d(transform_);
	//ワールドトランスフォームの更新
	worldTransform_->Update();
	//UVTransform
	materialData_->uvMatrix = Rendering::MakeUVAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
	//テキストのテクスチャの取得
}

//描画
void Text::Draw() {
	//2Dオブジェクトの共通部分
	Object2dCommon::GetInstance()->DrawSetting();
	//PSOの設定
	auto pso = Object2dCommon::GetInstance()->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
	//ワールドトランスフォームの描画
	worldTransform_->Draw();
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());//material
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSRVHandleGPU(textData_.textKey));
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(textData_.vertices.size()), 1, 0, 0, 0);
}

//終了
void Text::Finalize() {
	delete worldTransform_;
	delete textRasterizer_;
}

//頂点データの初期化
void Text::InitializeVertexData() {
	//左上
	textData_.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	//右上
	textData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	//左下
	textData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });

	//左下
	textData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	//右上
	textData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	//右下
	textData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
}

//インデックスリソースの生成
void Text::CreateIndexResource() {
	//Index用(3dGameObject)
	indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * textData_.vertices.size());
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * textData_.vertices.size());
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	//IndexResourceにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	for (int i = 0; i < textData_.vertices.size(); i++) {
		indexData_[i] = i; indexData_[i + 1] = i + 1; indexData_[i + 2] = i + 2;
		indexData_[i + 3] = i + 1; indexData_[i + 4] = i + 3; indexData_[i + 5] = i + 2;
	}
}

//頂点データの生成
void Text::CreateVertexResource() {
	//頂点データの初期化
	InitializeVertexData();
	//頂点リソースを生成
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(VertexData) * textData_.vertices.size());
	//VertexBufferViewを作成する(頂点バッファービュー)
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * textData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, textData_.vertices.data(), sizeof(VertexData) * textData_.vertices.size());//頂点データをリソースにコピー
}

//マテリアルデータの初期化
void Text::InitializeMaterialData() {
	//色を書き込む
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvMatrix = Matrix4x4::Identity4x4();
}

//マテリアルリソースの生成
void Text::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}

//テキストのテクスチャを取得または生成
void Text::AcquireTextTexture() {
	//CPUBitmapを作成
	CpuBitmap cpuBitmap = textRasterizer_->RenderTextToCpuBitmap(StringUtility::ConvertString(textStyle_.text), static_cast<uint32_t>(transform_.scale.x), static_cast<uint32_t>(transform_.scale.y), StringUtility::ConvertString(textStyle_.font), textStyle_.size, textStyle_.color);

	//テクスチャマネージャーに登録
	TextureManager::GetInstance()->UpdateTextureFromMemotyBGRA(textData_.textKey, cpuBitmap.bgra.data(), cpuBitmap.width, cpuBitmap.height, cpuBitmap.stride);
}
