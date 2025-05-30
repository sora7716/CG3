#include "ParticleEmit.h"
#include "engine/base/DirectXBase.h"
#include "engine/objectCommon/ParticleCommon.h"
#include "engine/gameObject/Camera.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/2d/TextureManager.h"
#include "engine/math/func/Math.h"
#include "engine/base/SRVManager.h"

//初期化
void ParticleEmit::Initialize(DirectXBase* directXBase) {
	//DirectXの基盤部分を記録する
	directXBase_ = directXBase;
	//ライトを生成
	ParticleCommon::GetInstance()->CreateDirectionLight();

	//カメラを設定
	camera_ = ParticleCommon::GetInstance()->GetDefaultCamera();

	//ワールドトランスフォームのリソースの生成
	CreateWorldTransformResource();
	//頂点リソースの生成
	CreateVertexResource();
	//マテリアルリソースの生成
	CreateMaterialResource();
	//ストラクチャバッファの生成
	CreateStructuredBuffer();

	//乱数エンジンの初期化
	std::random_device seedGenerator;
	std::mt19937 randomEngine_; (seedGenerator);
	//パーティクルの初期化
	for (uint32_t i = 0; i < kNumMaxInstance; i++) {
		particles_[i] = MakeNewParticle(randomEngine_);
		instancingData_[i].color = particles_[i].color;
	}
}

//更新
void ParticleEmit::Update() {
	//生存しているパーティクルの数を0に初期化
	numInstance = 0;
	//パーティクルの全体的に更新
	for (uint32_t i = 0; i < kNumMaxInstance; i++) {
		//移動
		particles_[i].transform.translate += particles_[i].velocity * deltaTime;
		//経過時間を足す
		particles_[i].currentTime += deltaTime;
		float alpha = 1.0f - (particles_[i].currentTime / particles_[i].lifeTime);
		instancingData_[i].color.w = alpha;
		//生きているパーティクルの数を記録
		numInstance++;
	}
	//ワールドトランスフォームの更新
	UpdateWorldTransform();
}

//描画
void ParticleEmit::Draw() {
	//描画準備
	ParticleCommon::GetInstance()->DrawSetting();
	//PSOの設定
	auto pso = ParticleCommon::GetInstance()->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());//material
	//ワールドトランスフォームの描画
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, SRVManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_));
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSRVHandleGPU(modelData_.material.textureFilePath));
	//描画(DrwaCall/ドローコール)
	directXBase_->GetCommandList()->DrawInstanced(static_cast<UINT>(modelData_.vertices.size()), numInstance, 0, 0);
}

//終了
void ParticleEmit::Finalize() {

}

//モデルデータの初期化
void ParticleEmit::InitializeModelData() {
	modelData_.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//左上
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//右上
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//左下
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//左下
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//右上
	modelData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//右下
	modelData_.material.textureFilePath = "engine/resources/textures/uvChecker.png";
}

//マテリアルデータの初期化
void ParticleEmit::InitializeMaterialData() {
	//色を書き込む
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = Math::MakeIdentity4x4();
}

//頂点リソースの生成
void ParticleEmit::CreateVertexResource() {
	//頂点データの初期化
	InitializeModelData();
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
void ParticleEmit::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}

//ワールドトランスフォームのリソースの生成
void ParticleEmit::CreateWorldTransformResource() {
	//座標変換行列リソースを作成する	
	instancingResource_ = directXBase_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	for (uint32_t i = 0; i < kNumMaxInstance; i++) {
		//単位行列を書き込んでおく
		instancingData_[i].WVP = Math::MakeIdentity4x4();
		instancingData_[i].World = Math::MakeIdentity4x4();
		instancingData_[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 初期色を白に設定
	}
}

//ワールドトランスフォームの更新
void ParticleEmit::UpdateWorldTransform() {
	for (uint32_t i = 0; i < kNumMaxInstance; i++) {
		//TransformからWorldMatrixを作る
		worldMatrix_ = Rendering::MakeAffineMatrix(particles_[i].transform.scale, particles_[i].transform.rotate, particles_[i].transform.translate);
		/*if (parent_) {
			worldMatrix_ = worldMatrix_ * parent_->worldMatrix_;
		}*/
		//wvpの書き込み
		if (camera_) {
			const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
			instancingData_[i].WVP = worldMatrix_ * viewProjectionMatrix;
		}
		else {
			instancingData_[i].WVP = worldMatrix_;
		}
		//ワールド行列を送信
		instancingData_[i].World = worldMatrix_;
	}
}

//ストラクチャバッファの生成
void ParticleEmit::CreateStructuredBuffer() {
	//テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);

	//ストラクチャバッファを生成
	srvIndex_ = SRVManager::GetInstance()->Allocate();
	SRVManager::GetInstance()->CreateSRVforStructuredBuffer(
		srvIndex_,
		instancingResource_.Get(),
		kNumMaxInstance,
		sizeof(ParticleForGPU)
	);
}

//パーティクルの生成
Particle ParticleEmit::MakeNewParticle(std::mt19937& randomEngine) {
	//位置と速度を[-1.0f,1.0f]でランダムに設定
	std::uniform_real_distribution<float>distribution(-1.0f, 1.0f);
	//パーティクルの初期化
	Particle particle;
	//拡縮
	particle.transform.scale = { 1.0f, 1.0f, 1.0f };
	//回転
	particle.transform.rotate = { 0.0f, 180.0f * rad, 0.0f };
	//位置
	particle.transform.translate = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
	//速度
	particle.velocity = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };

	//色の値を[0.0f,1.0f]でランダムに設定
	std::uniform_real_distribution<float>distColor(0.0f, 1.0f);
	//色
	particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine),1.0f };

	//生存時間
	std::uniform_real_distribution<float>distTime(1.0f, 3.0f);
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0.0f;
	return particle;
}
