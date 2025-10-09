#define NOMINMAX
#include <algorithm>
#include "ParticleSystem.h"
#include "engine/base/DirectXBase.h"
#include "engine/particle/ParticleCommon.h"
#include "engine/camera/Camera.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/2d/TextureManager.h"
#include "engine/math/func/Math.h"
#include "engine/base/SRVManager.h"
#include "engine/3d/ModelManager.h"

//初期化
void ParticleSystem::Initialize(DirectXBase* directXBase, const std::string& textureName, Model* model) {
	//DirectXの基盤部分を記録する
	directXBase_ = directXBase;

	//エミッター
	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize(model);

	//ワールドトランスフォームのリソースの生成
	CreateWorldTransformResource();

	//頂点リソースの生成
	CreateVertexResource();

	//テクスチャファイルの記録
	modelData_.material.textureFilePath = "engine/resources/textures/" + textureName;
	//テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);

	//マテリアルリソースの生成
	CreateMaterialResource();

	//インデックスリソースの生成
	CreateIndexResource();

	//ストラクチャバッファの生成
	CreateStructuredBuffer();
}

//更新
void ParticleSystem::Update() {
	emitter_->Update(instancingData_);
}

//描画
void ParticleSystem::Draw() {
	//描画準備
	ParticleCommon::GetInstance()->DrawSetting();
	//PSOの設定
	auto pso = ParticleCommon::GetInstance()->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());//material
	//ワールドトランスフォームの描画
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, SRVManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_));
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSRVHandleGPU(modelData_.material.textureFilePath));
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(modelData_.vertices.size()), emitter_->GetNumInstance(), 0, 0, 0);
}


//デバッグ
void ParticleSystem::Debug() {
	//#ifdef  USE_IMGUI
	//	ImGui::Text("size:%d", particles_.size());
	//	ImGui::Text("instance:%d", numInstance_);
	//	ImGui::DragFloat3("acceleration", &accelerationField_.acceleration.x, 0.1f);
	//	ImGui::DragFloat3("translate", &emitter_.transform.translate.x, 0.1f);
	//#endif //USE_IMGUI
}

//終了
void ParticleSystem::Finalize() {

}

//テクスチャ名のゲッター
std::string ParticleSystem::GetTextureName() {
	return modelData_.material.textureFilePath;
}

//カメラのセッター
void ParticleSystem::SetCamera(Camera* camera) {
	emitter_->SetCamera(camera);
}

//モデルデータの初期化
void ParticleSystem::InitializeQuadModelData() {
	modelData_.vertices.push_back({
		.position = {1.0f,1.0f,0.0f,1.0f},
		.texcoord = {0.0f,0.0f},
		.normal = {0.0f,0.0f,1.0f}
		});//左上
	modelData_.vertices.push_back({
		.position = {-1.0f,1.0f,0.0f,1.0f},
		.texcoord = {1.0f,0.0f},
		.normal = {0.0f,0.0f,1.0f}
		});//右上
	modelData_.vertices.push_back({
		.position = {1.0f,-1.0f,0.0f,1.0f},
		.texcoord = {0.0f,1.0f},
		.normal = {0.0f,0.0f,1.0f}
		});//左下
	modelData_.vertices.push_back({
		.position = {1.0f,-1.0f,0.0f,1.0f},
		.texcoord = {0.0f,1.0f},
		.normal = {0.0f,0.0f,1.0f}
		});//左下
	modelData_.vertices.push_back({
		.position = {-1.0f,1.0f,0.0f,1.0f},
		.texcoord = {1.0f,0.0f},
		.normal = {0.0f,0.0f,1.0f}
		});//右上
	modelData_.vertices.push_back({
		.position = {-1.0f,-1.0f,0.0f,1.0f},
		.texcoord = {1.0f,1.0f},
		.normal = {0.0f,0.0f,1.0f}
		});//右下
}

//マテリアルデータの初期化
void ParticleSystem::InitializeMaterialData() {
	//色を書き込む
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = Matrix4x4::Identity4x4();
}

//頂点リソースの生成
void ParticleSystem::CreateVertexResource() {
	//頂点データの初期化
	if (emitter_->GetModel()) {
		//モデルがあればモデルデータを取得
		modelData_ = emitter_->GetModel()->GetModelData();
	} else {
		//モデルがなければ四角形を生成
		InitializeQuadModelData();
	}
	//頂点リソースを生成
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
	//VertexBufferViewを作成する(頂点バッファービュー)
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());//頂点データをリソースにコピー
}

//マテリアルリソースの生成
void ParticleSystem::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}

void ParticleSystem::CreateIndexResource() {
	//Index用(3dGameObject)
	indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * modelData_.vertices.size());
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.vertices.size());
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	//IndexResourceにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	for (int i = 0; i < modelData_.vertices.size(); i++) {
		indexData_[i] = i; indexData_[i + 1] = i + 1; indexData_[i + 2] = i + 2;
		indexData_[i + 3] = i + 1; indexData_[i + 4] = i + 3; indexData_[i + 5] = i + 2;
	}
}

//ワールドトランスフォームのリソースの生成
void ParticleSystem::CreateWorldTransformResource() {
	//座標変換行列リソースを作成する	
	instancingResource_ = directXBase_->CreateBufferResource(sizeof(ParticleForGPU) * ParticleEmitter::kNumMaxInstance);
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	for (uint32_t i = 0; i < ParticleEmitter::kNumMaxInstance; i++) {
		//単位行列を書き込んでおく
		instancingData_[i].WVP = Matrix4x4::Identity4x4();
		instancingData_[i].World = Matrix4x4::Identity4x4();
		instancingData_[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 初期色を白に設定
	}
}

//ストラクチャバッファの生成
void ParticleSystem::CreateStructuredBuffer() {
	//ストラクチャバッファを生成
	srvIndex_ = SRVManager::GetInstance()->Allocate() + TextureManager::kSRVIndexTop;
	SRVManager::GetInstance()->CreateSRVForStructuredBuffer(
		srvIndex_,
		instancingResource_.Get(),
		ParticleEmitter::kNumMaxInstance,
		sizeof(ParticleForGPU)
	);
}