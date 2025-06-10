#include "SpriteCommon.h"
#include "engine/base/DirectXBase.h"
#include "engine/camera/Camera.h"
#include "engine/base/GraphicsPipeline.h"
#include "engine/2d/TextureManager.h"
#include <cassert>
using namespace Microsoft::WRL;

//インスタンスのゲッター
SpriteCommon* SpriteCommon::GetInstance() {
	assert(!isFinalize && "GetInstance() called after Finalize()");
	if (instance == nullptr) {
		instance = new SpriteCommon();
	}
	return instance;
}

//初期化
void SpriteCommon::Initialize(DirectXBase* directXBase) {
	//Nullチェック
	assert(directXBase);
	//引数を受け取ってメンバ変数に記録する
	directXBase_ = directXBase;
	//ブレンド
	blend_ = new Blend();
	//グラフィックスパイプラインの生成と初期化
	makeGraphicsPipeline_ = new GraphicsPipeline();
	//シェーダを設定
	makeGraphicsPipeline_->SetVertexShaderFileName(L"Object3d.VS.hlsl");
	makeGraphicsPipeline_->SetPixelShaderFileName(L"Object3d.PS.hlsl");
	makeGraphicsPipeline_->Initialize(directXBase_);
	//ルートシグネイチャの記録
	rootSignature_ = makeGraphicsPipeline_->GetRootSignature();
	//グラフィックスパイプラインステートの記録
	graphicsPipelineStates_ = makeGraphicsPipeline_->GetGraphicsPipelines();
}

//共通描画設定
void SpriteCommon::DrawSetting() {
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//テクスチャの読み込み
void SpriteCommon::LoadTexture(const std::string& filename) {
	TextureManager::GetInstance()->LoadTexture(filename);
}

//光源の生成
void SpriteCommon::CreateDirectionLight() {
	//光源のリソースを作成
	directionalLightResource_ = directXBase_->CreateBufferResource(sizeof(DirectionalLight));
	//光源データの書きこみ
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_->intensity = 1.0f;
}

//DirectionalLightのセッター
void SpriteCommon::SetDirectionalLightData(const DirectionalLight& directionalLightData) {
	directionalLightData_->color = directionalLightData.color;
	directionalLightData_->direction = directionalLightData.direction;
	directionalLightData_->intensity = directionalLightData.intensity;
}

//DirectionalLightのリソースのゲッター
ID3D12Resource* SpriteCommon::GetDirectionalLightResource()const {
	return directionalLightResource_.Get();
}

//DirectXの基盤部分のゲッター
DirectXBase* SpriteCommon::GetDirectXBase() {
	return directXBase_;
}

//グラフィックパイプラインのゲッター
std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> SpriteCommon::GetGraphicsPipelineStates() const {
	return graphicsPipelineStates_;
}

//終了
void SpriteCommon::Finalize() {
	delete blend_;
	delete makeGraphicsPipeline_;
	delete instance;
	instance = nullptr;
	isFinalize = true;
}