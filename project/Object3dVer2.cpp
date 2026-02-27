#include "Object3dVer2.h"
#include "engine/camera/Camera.h"
#include "engine/base/DirectXBase.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/math/func/Math.h"
#include "engine/3d/ModelManager.h"
#include "engine/worldTransform/WorldTransform.h"
#include "engine/math/func/Rendering.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/base/SRVManager.h"
#include "engine/2d/TextureManager.h"
#include "engine/base/Blend.h"
#include "engine/base/GraphicsPipeline.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include "Field.h"

//コンストラクタ
Object3dVer2::Object3dVer2() {
}

//デストラクタ
Object3dVer2::~Object3dVer2() {
}

//初期化
void Object3dVer2::Initialize(Object3dCommon* object3dCommon, Camera* camera, const std::string& modelName) {
	//3Dオブジェクトの共通部分の記録
	object3dCommon_ = object3dCommon;

	//DirectXの基盤部分の記録
	directXBase_ = object3dCommon_->GetDirectXBase();

	//SRVマネージャーを記録
	srvManager_ = object3dCommon_->GetSRVManager();

	//カメラを記録
	camera_ = camera;

	//ブレンド
	blend_ = std::make_unique<Blend>();

	//グラフィックスパイプラインの生成と初期化
	makeGraphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	//シェーダを設定
	//頂点シェーダ
	makeGraphicsPipeline_->SetVertexShaderFileName(L"MapChip.VS.hlsl");

	//ピクセルシェーダ
	makeGraphicsPipeline_->SetPixelShaderFileName(L"MapChip.PS.hlsl");

	//デプスステンシルステート
	directXBase_->InitializeDepthStencilForObject3d();

	//DirectXを記録
	makeGraphicsPipeline_->SetDirectXBase(directXBase_);

	//シグネイチャBlobの初期化
	makeGraphicsPipeline_->CreateRootSignatureBlobForMapChip();

	//ルートシグネイチャの保存
	makeGraphicsPipeline_->CreateRootSignature();

	//インプットレイアウト
	makeGraphicsPipeline_->InitializeInputLayoutDesc();

	//ラスタライザステート
	makeGraphicsPipeline_->InitializeRasterizerSatate();

	//頂点シェーダBlob
	makeGraphicsPipeline_->CompileVertexShader();

	//ピクセルシェーダBlob
	makeGraphicsPipeline_->CompilePixelShader();

	//PSO
	for (uint32_t i = 0; i < static_cast<int32_t>(BlendMode::kCountOfBlendMode); i++) {
		//ブレンドステート
		makeGraphicsPipeline_->InitializeBlendState(i);
		//グラフィックスパイプラインの生成
		graphicsPipelineStates_[i] = makeGraphicsPipeline_->CreateGraphicsPipeline(directXBase_->GetDepthStencil());
	}

	//ルートシグネイチャの記録
	rootSignature_ = makeGraphicsPipeline_->GetRootSignature();

	//ライティング
	//平行光源の生成
	CreateDirectionLight();

	//点光源の生成
	CreatePointLight();

	//点光源のSRVの生成
	CreateStructuredBufferForPoint();

	//スポットライトの生成
	CreateSpotLight();

	//スポットライトのSRVの生成
	CreateStructuredBufferForSpot();

	//カメラリソースの生成
	CreateCameraResource(camera_->GetTranslate());

	transforms_[0].scale = Vector3::MakeAllOne();
	transforms_[0].translate = {};
	transforms_[1].scale = Vector3::MakeAllOne();
	transforms_[1].translate = { 1.0f,0.0f,0.0f };

	//モデルデータの読み込み
	modelData_ = object3dCommon_->GetModelManager()->FindModel(modelName)->GetModelData();

	//テクスチャの読み込み
	object3dCommon_->GetTextureManager()->LoadTexture(modelData_.material.textureFilePath);

	//wvpリソースの生成
	CreateTransformationMatrixResource();

	//wvpのSRVの生成
	CreateStructuredBufferForWvp();

	//頂点リソースの生成
	CreateVertexResource();

	//インデックスリソースのの生成
	CreateIndexResource();

	//マテリアルリソースのの生成
	CreateMaterialResource();

	//uv座標
	uvTransform_ = { {1.0f,1.0f},0.0f,{0.0f,0.0f} };

	//マテリアルの初期化
	materialData_.color = Vector4::MakeWhiteColor();
	materialData_.enableLighting = true;
	materialData_.shininess = 10.0f;

	//DirectionalLightの初期化
	directionalLightData_.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_.direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_.intensity = 1.0f;
	directionalLightData_.isLambert = false;
	directionalLightData_.isBlinnPhong = true;
	directionalLightData_.enableDirectionalLighting = true;

	//PointLightの初期化
	for (int i = 0; i < kMaxLightCount; i++) {
		pointLightDataList_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		pointLightDataList_[i].position = {};
		pointLightDataList_[i].intensity = 1.0f;
		pointLightDataList_[i].distance = 7.0f;
		pointLightDataList_[i].decay = 4.0f;
		pointLightDataList_[i].isBlinnPhong = false;
		pointLightDataList_[i].enablePointLighting = false;
	}

	//SpotLightの初期化
	for (int i = 0; i < kMaxLightCount; i++) {
		spotLightDataList_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		spotLightDataList_[i].position = { 2.0f,1.25f,0.0f };
		spotLightDataList_[i].distance = 7.0f;
		spotLightDataList_[i].direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
		spotLightDataList_[i].intensity = 4.0f;
		spotLightDataList_[i].decay = 2.0f;
		spotLightDataList_[i].cosFolloffStart = 1.0f;
		spotLightDataList_[i].cosAngle = std::cos(Math::kPi / 3.0f);
		spotLightDataList_[i].isBlinnPhong = false;
		spotLightDataList_[i].enableSpotLighting = false;
	}
}


//更新
void Object3dVer2::Update() {
	//マテリアルの更新
	materialPtr_->color = materialData_.color;
	materialPtr_->enableLighting = materialData_.enableLighting;
	materialPtr_->shininess = materialData_.shininess;

	//ライトの更新
	*directionalLightPtr_ = directionalLightData_;
	for (int i = 0; i < kMaxLightCount; i++) {
		pointLightPtr_[i] = pointLightDataList_[i];
		spotLightPtr_[i] = spotLightDataList_[i];
	}

	//ワールド行列の更新
	UpdateTransform();

	//UVのワールド行列の更新
	UVUpdateTransform();
}

//デバッグ
void Object3dVer2::Debug() {
#ifdef USE_IMGUI
	ImGui::ColorEdit4("color", &materialData_.color.x);
	ImGui::DragFloat("shininess", &materialData_.shininess, 0.1f);
#endif // USE_IMGUI
	ImGuiManager::CheckBoxToInt("enableLighting", materialData_.enableLighting);
}

//マテリアルのゲッター
Material Object3dVer2::GetMaterial() {
	return materialData_;
}

//描画
void Object3dVer2::Draw() {
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//カメラCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_.Get()->GetGPUVirtualAddress());

	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//PSOの設定
	auto pso = graphicsPipelineStates_[static_cast<int32_t>(blendMode_)].Get();

	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);

	//座標変換行列SRVの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(srvIndexWvp_));//wvp

	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, srvManager_->GetGPUDescriptorHandle(srvIndexPoint_));

	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, srvManager_->GetGPUDescriptorHandle(srvIndexSpot_));

	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定

	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定

	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, object3dCommon_->GetTextureManager()->GetSRVHandleGPU(modelData_.material.textureFilePath));

	//描画
	directXBase_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.vertices.size()), kMaxBlockCount, 0, 0, 0);
}

//カメラのセッター
void Object3dVer2::SetCamera(Camera* camera) {
	camera_ = camera;
	CreateCameraResource(camera->GetTranslate());
}

//ブレンドモードのセッター
void Object3dVer2::SetBlendMode(const BlendMode& blendMode) {
	blendMode_ = blendMode;
}

//平行光源のセッター
void Object3dVer2::SetDirectionalLight(const DirectionalLight& directionalLight) {
	directionalLightData_ = directionalLight;
}

//点光源のセッター
void Object3dVer2::SetPontLight(const PointLight* pointLight) {
	for (uint32_t i = 0; i < kMaxLightCount; i++) {
		pointLightDataList_[i] = pointLight[i];
	}
}

//スポットライトのセッター
void Object3dVer2::SetSpotLight(const SpotLight* spotLight) {
	for (uint32_t i = 0; i < kMaxLightCount; i++) {
		spotLightDataList_[i] = spotLight[i];
	}
}

//テクスチャのセッター
void Object3dVer2::SetTexture(const std::string& textureName) {
	modelData_.material.textureFilePath = "engine/resources/textures/" + textureName;
	object3dCommon_->GetTextureManager()->LoadTexture(modelData_.material.textureFilePath);
}

//カメラの位置のセッター
void Object3dVer2::SetCameraForGPU(const Vector3& cameraTranslate) {
	cameraForGPU_->worldPosition = cameraTranslate;
}

//マテリアルのセッター
void Object3dVer2::SetMaterial(const Material& material) {
	materialData_ = material;
}

//カメラリソースの生成
void Object3dVer2::CreateCameraResource(const Vector3& cameraTranslate) {
	//光源のリソースを作成
	cameraResource_ = directXBase_->CreateBufferResource(sizeof(CameraForGPU));
	//光源データの書きこみ
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPU_));
	cameraForGPU_->worldPosition = cameraTranslate;
}

//座標変換行列リソースの生成
void Object3dVer2::CreateTransformationMatrixResource() {
	// 配列サイズで確保
	wvpResource_ = directXBase_->CreateBufferResource(sizeof(TransformationMatrix) * kMaxBlockCount);
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpPtr_));
	//単位行列を書き込んでおく
	for (uint32_t i = 0; i < kMaxBlockCount; i++) {
		wvpPtr_[i].wvp = Matrix4x4::Identity4x4();
		wvpPtr_[i].world = Matrix4x4::Identity4x4();
		wvpPtr_[i].worldInverseTranspose = Matrix4x4::Identity4x4();
	}
}

//座標変換行列リソースのストラクチャバッファの生成
void Object3dVer2::CreateStructuredBufferForWvp() {
	//ストラクチャバッファを生成
	srvIndexWvp_ = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		srvIndexWvp_,
		wvpResource_.Get(),
		kMaxBlockCount,
		sizeof(TransformationMatrix)
	);
}

//座標の更新
void Object3dVer2::UpdateTransform() {
	for (uint32_t i = 0; i < kMaxBlockCount; i++) {

		//TransformからWorldMatrixを作る
		wvpDataList_[i].world = Rendering::MakeAffineMatrix(transforms_[i]);

		//wvpの書き込み
		if (camera_) {
			const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
			wvpDataList_[i].wvp = wvpDataList_[i].world * viewProjectionMatrix;
		} else {
			wvpDataList_[i].wvp = wvpDataList_[i].world;
		}

		//逆行列の転置行列
		wvpDataList_[i].worldInverseTranspose = wvpDataList_[i].world.InverseTranspose();

		//ポインタに送信
		wvpPtr_[i] = wvpDataList_[i];
	}

}

//uv変換
void Object3dVer2::UVUpdateTransform() {
	materialPtr_->uvMatrix = Rendering::MakeUVAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
}

//頂点リソースの生成
void Object3dVer2::CreateVertexResource() {
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

//インデックスリソースの生成
void Object3dVer2::CreateIndexResource() {
	//Index用(3dGameObject)
	indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * modelData_.vertices.size());
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.vertices.size());
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	//IndexResourceにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexPtr_));
	for (int i = 0; i < modelData_.vertices.size(); i++) {
		indexPtr_[i] = i; indexPtr_[i + 1] = i + 1; indexPtr_[i + 2] = i + 2;
		indexPtr_[i + 3] = i + 1; indexPtr_[i + 4] = i + 3; indexPtr_[i + 5] = i + 2;
	}
}

//マテリアルリソースの生成
void Object3dVer2::CreateMaterialResource() {
	//マテリアル用のリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialPtr_));
	//色を書き込む
	materialPtr_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialPtr_->enableLighting = true;
	materialPtr_->uvMatrix = Matrix4x4::Identity4x4();
	materialPtr_->shininess = 0.1f;
}

//平行光源の生成
void Object3dVer2::CreateDirectionLight() {
	//光源のリソースを作成
	directionalLightResource_ = directXBase_->CreateBufferResource(sizeof(DirectionalLight));
	//光源データの書きこみ
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightPtr_));
	directionalLightPtr_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightPtr_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightPtr_->intensity = 10.0f;
	directionalLightPtr_->isLambert = false;
	directionalLightPtr_->isBlinnPhong = true;
	directionalLightPtr_->enableDirectionalLighting = true;
}

//点光源の生成
void Object3dVer2::CreatePointLight() {
	// 配列サイズで確保
	pointLightResource_ = directXBase_->CreateBufferResource(sizeof(PointLight) * kMaxLightCount);

	//配列としてMap
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightPtr_));

	// とりあえず全部初期化（必要なら0番だけGUI値を入れる）
	for (uint32_t i = 0; i < kMaxLightCount; ++i) {
		pointLightPtr_[i].color = { 1,1,1,1 };
		pointLightPtr_[i].position = { 0,-1,0 };
		pointLightPtr_[i].intensity = 10.0f;
		pointLightPtr_[i].distance = 7.0f;
		pointLightPtr_[i].decay = 2.0f;
		pointLightPtr_[i].isBlinnPhong = true;
		pointLightPtr_[i].enablePointLighting = false;
	}
}

//点光源のストラクチャバッファの生成
void Object3dVer2::CreateStructuredBufferForPoint() {
	//ストラクチャバッファを生成
	srvIndexPoint_ = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		srvIndexPoint_,
		pointLightResource_.Get(),
		kMaxLightCount,
		sizeof(PointLight)
	);
}

//スポットライトの生成
void Object3dVer2::CreateSpotLight() {
	// 配列サイズで確保
	spotLightResource_ = directXBase_->CreateBufferResource(sizeof(SpotLight) * kMaxLightCount);

	//光源データの書きこみ
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightPtr_));

	// とりあえず全部初期化（必要なら0番だけGUI値を入れる）
	for (uint32_t i = 0; i < kMaxLightCount; ++i) {
		spotLightPtr_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		spotLightPtr_[i].position = { 2.0f,1.25f,0.0f };
		spotLightPtr_[i].distance = 7.0f;
		spotLightPtr_[i].direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
		spotLightPtr_[i].intensity = 4.0f;
		spotLightPtr_[i].decay = 2.0f;
		spotLightPtr_[i].cosAngle = std::cos(Math::kPi / 3.0f);
		spotLightPtr_[i].cosFolloffStart = 0.0f;
		spotLightPtr_[i].isBlinnPhong = true;
		spotLightPtr_[i].enableSpotLighting = false;
	}
}

//スポットライトのストラクチャバッファの生成
void Object3dVer2::CreateStructuredBufferForSpot() {
	//ストラクチャバッファを生成
	srvIndexSpot_ = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		srvIndexSpot_,
		spotLightResource_.Get(),
		kMaxLightCount,
		sizeof(SpotLight)
	);
}