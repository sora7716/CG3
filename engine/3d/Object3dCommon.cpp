#include "Object3dCommon.h"
#include "engine/base/DirectXBase.h"
#include "engine/camera/Camera.h"
#include "engine/base/GraphicsPipeline.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/base/SRVManager.h"
#include "engine/2d/TextureManager.h"
#include <cassert>
#include "engine/math/func/Math.h"
#include "engine/debug/GlobalVariables.h"
using namespace Microsoft::WRL;

//デストラクタ
Object3dCommon::~Object3dCommon() {
	delete blend_;
	delete makeGraphicsPipeline_;
}

//初期化
void Object3dCommon::Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, ModelManager* modelManager, WireframeObject3dCommon* wireframeObject3dCommon) {
	//DirectXの基盤を受け取る
	directXBase_ = directXBase;
	//SRVマネージャーを受け取る
	srvManager_ = srvManager;
	//テクスチャマネージャーを受け取る
	textureManager_ = textureManager;
	//モデルマネージャー
	modelManager_ = modelManager;
	//ワイヤーフレームオブジェクトの共通部分
	wireframeObject3dCommon_ = wireframeObject3dCommon;
	//ブレンド
	blend_ = new Blend();
	//グラフィックスパイプラインの生成と初期化
	makeGraphicsPipeline_ = new GraphicsPipeline();
	//シェーダを設定
	makeGraphicsPipeline_->SetVertexShaderFileName(L"Object3d.VS.hlsl");
	makeGraphicsPipeline_->SetPixelShaderFileName(L"Object3d.PS.hlsl");
	//デプスステンシルステート
	directXBase_->InitializeDepthStencilForObject3d();
	//DirectXを記録
	makeGraphicsPipeline_->SetDirectXBase(directXBase);
	//シグネイチャBlobの初期化
	makeGraphicsPipeline_->CreateRootSignatureBlobForObject3d();
	//ルートシグネイチャの保存
	makeGraphicsPipeline_->CreateRootSignature();
	//インプットレイアウト
	makeGraphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	makeGraphicsPipeline_->InitializeRasterizerSatate(FillMode::kSolid);
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
	}	//ルートシグネイチャの記録
	rootSignature_ = makeGraphicsPipeline_->GetRootSignature();

	//DirectionalLightの初期化
	directionalLightData_.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_.direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_.intensity = 1.0f;
	directionalLightData_.isLambert = false;
	directionalLightData_.isBlinnPhong = true;
	directionalLightData_.enableDirectionalLighting = false;

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

	pointLightDataList_[0].position = {};
	pointLightDataList_[0].intensity = 1.5f;
	pointLightDataList_[0].distance = 9.5f;
	pointLightDataList_[0].decay = 15.0f;
	pointLightDataList_[0].isBlinnPhong = true;
	pointLightDataList_[0].enablePointLighting = true;

	////SpotLightの初期化
	//for (int i = 0; i < kMaxLightCount; i++) {
	//	spotLightDataList_[i].color = { 1.0f,1.0f,1.0f,1.0f };
	//	spotLightDataList_[i].position = { 2.0f,1.25f,0.0f };
	//	spotLightDataList_[i].distance = 7.0f;
	//	spotLightDataList_[i].direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
	//	spotLightDataList_[i].intensity = 4.0f;
	//	spotLightDataList_[i].decay = 2.0f;
	//	spotLightDataList_[i].cosFolloffStart = 1.0f;
	//	spotLightDataList_[i].cosAngle = std::cos(Math::kPi / 3.0f);
	//	spotLightDataList_[i].isBlinnPhong = false;
	//	spotLightDataList_[i].enableSpotLighting = false;
	//}

	//ライティング
	//平行光源の生成
	CreateDirectionLight();
	//点光源の生成
	CreatePointLight();
	CreateStructuredBufferForPoint();
	//スポットライトの生成
	CreateSpotLight();
	CreateStructuredBufferForSpot();

	//調整項目に設定
	AddItemForPointLight(pointLightGroupNames_[0].c_str(), pointLightDataList_[0]);
	//AddItemForSpotLight(spotLightGroupNames_[0].c_str(), spotLightDataList_[0]);
}

//更新
void Object3dCommon::Update() {
	//調整項目を適応
	//ApplyGlobalVariablesForPointLight(pointLightGroupNames_[0].c_str(), pointLightDataList_[0]);
	//ApplyGlobalVariablesForSpotLight(spotLightGroupNames_[0].c_str(), spotLightDataList_[0]);

	//ライトデータを転送
	*directionalLightPtr_ = directionalLightData_;

	pointLightDataList_[0].position = pointLightPos_;
	for (int32_t i = 0; i < kMaxLightCount; i++) {
		pointLightPtr_[i] = pointLightDataList_[i];
	}

	//スポットライトのポインタにデータを転送
	int32_t index = 0;
	for (auto& [key, value] : spotLightDataList_) {
		spotLightPtr_[index] = value;
		index++;
	}
}

//共通描画設定
void Object3dCommon::DrawSetting() {
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	//カメラCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_.Get()->GetGPUVirtualAddress());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//デバッグ
void Object3dCommon::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("Lighting");
	ImGui::ColorEdit4("directional.color", &directionalLightData_.color.x);
	ImGui::DragFloat3("directional.direction", &directionalLightData_.direction.x, 0.1f);
	ImGui::DragFloat("directional.intensity", &directionalLightData_.intensity, 0.1f);
	ImGuiManager::CheckBoxToInt("directional.isLambert", directionalLightData_.isLambert);
	ImGuiManager::CheckBoxToInt("directional.isBlingPhong", directionalLightData_.isBlinnPhong);
	ImGuiManager::CheckBoxToInt("directional.enableDirectionalLight", directionalLightData_.enableDirectionalLighting);

	ImGui::DragFloat("pointLight.decay", &pointLightDataList_[0].decay, 0.1f);
	ImGui::DragFloat("pointLight.distance", &pointLightDataList_[0].distance, 0.1f);
	ImGui::DragFloat("pointLight.intensity", &pointLightDataList_[0].intensity, 0.1f);
	ImGui::End();
#endif // USE_IMGUi
}

//カメラリソースの生成
void Object3dCommon::CreateCameraResource(const Vector3& cameraTranslate) {
	//光源のリソースを作成
	cameraResource_ = directXBase_->CreateBufferResource(sizeof(CameraForGPU));
	//光源データの書きこみ
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPU_));
	cameraForGPU_->worldPosition = cameraTranslate;
}

//カメラの位置のセッター
void Object3dCommon::SetCameraForGPU(const Vector3& cameraTranslate) {
	cameraForGPU_->worldPosition = cameraTranslate;
}

//DirectionalLightのリソースのゲッター
ID3D12Resource* Object3dCommon::GetDirectionalLightResource()const {
	return directionalLightResource_.Get();
}

//PointLightのリソースのゲッター
ID3D12Resource* Object3dCommon::GetPointLightResource() const {
	return pointLightResource_.Get();
}

//SpotLightのリソースのゲッター
ID3D12Resource* Object3dCommon::GetSpotLightResource() const {
	return spotLightResource_.Get();
}

//DirectXの基盤のゲッター
DirectXBase* Object3dCommon::GetDirectXBase() const {
	return directXBase_;
}

//SRVマネージャーのゲッター
SRVManager* Object3dCommon::GetSRVManager() const {
	return srvManager_;
}

//テクスチャマネージャーのゲッター
TextureManager* Object3dCommon::GetTextureManager() const {
	return textureManager_;
}

//モデルマネージャーのゲッター
ModelManager* Object3dCommon::GetModelManager() const {
	return modelManager_;
}

//ワイヤーフレームオブジェクトの共通部分のゲッター
WireframeObject3dCommon* Object3dCommon::GetWireframeObject3dCommon() const {
	return wireframeObject3dCommon_;
}

//グラフィックパイプラインのゲッター
std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> Object3dCommon::GetGraphicsPipelineStates() const {
	return graphicsPipelineStates_;
}

// デフォルトカメラのセッター
void Object3dCommon::SetDefaultCamera(Camera* camera) {
	defaultCamera_ = camera;
}

// デフォルトカメラのゲッター
Camera* Object3dCommon::GetDefaultCamera() const {
	return defaultCamera_;
}

//SRVインデックスのゲッター(PointLight)
uint32_t Object3dCommon::GetSrvIndexPoint() const {
	return srvIndexPoint_;
}

//SRVインデックスのゲッター(SpotLight)
uint32_t Object3dCommon::GetSrvIndexSpot() const {
	return srvIndexSpot_;
}

//平行光源のゲッター
const DirectionalLight& Object3dCommon::GetDirectionalLight() const {
	// TODO: return ステートメントをここに挿入します
	return directionalLightData_;
}

//点光源のセッター
PointLight* Object3dCommon::GetPointLight() {
	return pointLightDataList_;
}

//スポットライトのゲッター
SpotLight* Object3dCommon::GetSpotLightPtr() {
	return spotLightPtr_;
}

//スポットライトを追加
void Object3dCommon::AddSpotLight(const std::string& name) {
	//読み込み済みならカメラを検索
	if (spotLightDataList_.contains(name)) {
		//読み込み済みなら早期return
		return;
	}
	//スポットライトを生成
	SpotLight spotLight = {};
	spotLight.color = { 1.0f,1.0f,1.0f,1.0f };
	spotLight.position = { 2.0f,1.25f,0.0f };
	spotLight.distance = 7.0f;
	spotLight.direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
	spotLight.intensity = 4.0f;
	spotLight.decay = 2.0f;
	spotLight.cosAngle = std::cos(Math::kPi / 3.0f);
	spotLight.cosFolloffStart = 1.0f;
	spotLight.isBlinnPhong = true;
	spotLight.enableSpotLighting = true;

	//スポットライトをmapコンテナに格納する
	spotLightDataList_.insert(std::make_pair(name, spotLight));
}

//スポットライトのセッター
void Object3dCommon::SetSpotLight(const std::string& name, const SpotLight& spotLight) {
	spotLightDataList_.at(name) = spotLight;
}

//スポットライトのゲッター
SpotLight& Object3dCommon::GetSpotLight(const std::string& name) {
	// TODO: return ステートメントをここに挿入します
	return spotLightDataList_.at(name);
}

//ポイントライトの位置
void Object3dCommon::SetPointLightPos(const Vector3& pointLightPos) {
	pointLightPos_ = pointLightPos;
}

//コンストラクタ
Object3dCommon::Object3dCommon(ConstructorKey) {
}

//平行光源の生成
void Object3dCommon::CreateDirectionLight() {
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
void Object3dCommon::CreatePointLight() {
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
void Object3dCommon::CreateStructuredBufferForPoint() {
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
void Object3dCommon::CreateSpotLight() {
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
		spotLightPtr_[i].cosFolloffStart = 1.0f;
		spotLightPtr_[i].isBlinnPhong = true;
		spotLightPtr_[i].enableSpotLighting = false;
	}
}

//スポットライトのストラクチャバッファの生成
void Object3dCommon::CreateStructuredBufferForSpot() {
	//ストラクチャバッファを生成
	srvIndexSpot_ = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		srvIndexSpot_,
		spotLightResource_.Get(),
		kMaxLightCount,
		sizeof(SpotLight)
	);
}

//グローバル変数に追加(PointLight)
void Object3dCommon::AddItemForPointLight(const char* groupName, const PointLight& pointLight) {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "color", pointLight.color);
	globalVariables->AddItem(groupName, "position", pointLight.position);
	globalVariables->AddItem(groupName, "intensity", pointLight.intensity);
	globalVariables->AddItem(groupName, "distance", pointLight.distance);
	globalVariables->AddItem(groupName, "decay", pointLight.decay);
	globalVariables->AddItem(groupName, "isBlinnPhong", pointLight.isBlinnPhong);
	globalVariables->AddItem(groupName, "enablePointLighting", pointLight.enablePointLighting);
}

//グローバル変数に追加(SpotLight)
void Object3dCommon::AddItemForSpotLight(const char* groupName, const SpotLight& spotLight) {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "color", spotLight.color);
	globalVariables->AddItem(groupName, "cosAngle", spotLight.cosAngle);
	globalVariables->AddItem(groupName, "cosFolloffStart", spotLight.cosFolloffStart);
	globalVariables->AddItem(groupName, "decay", spotLight.decay);
	globalVariables->AddItem(groupName, "direction", spotLight.direction);
	globalVariables->AddItem(groupName, "distance", spotLight.distance);
	globalVariables->AddItem(groupName, "enableSpotLighting", spotLight.enableSpotLighting);
	globalVariables->AddItem(groupName, "intensity", spotLight.intensity);
	globalVariables->AddItem(groupName, "isBlinnPhong", spotLight.isBlinnPhong);
	globalVariables->AddItem(groupName, "position", spotLight.position);
}

//グローバル変数を適用(PointLight)
void Object3dCommon::ApplyGlobalVariablesForPointLight(const char* groupName, PointLight& pointLight) {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	pointLight.color = globalVariables->GetValue<Vector4>(groupName, "color");
	pointLight.decay = globalVariables->GetValue<float>(groupName, "decay");
	pointLight.distance = globalVariables->GetValue<float>(groupName, "distance");
	pointLight.enablePointLighting = globalVariables->GetValue<int32_t>(groupName, "enablePointLighting");
	pointLight.intensity = globalVariables->GetValue<float>(groupName, "intensity");
	pointLight.isBlinnPhong = globalVariables->GetValue<int32_t>(groupName, "isBlinnPhong");
	pointLight.position = globalVariables->GetValue<Vector3>(groupName, "position");
}

//グローバル変数を適用(SpotLight)
void Object3dCommon::ApplyGlobalVariablesForSpotLight(const char* groupName, SpotLight& spotLight) {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	spotLight.color = globalVariables->GetValue<Vector4>(groupName, "color");
	spotLight.cosAngle = globalVariables->GetValue<float>(groupName, "cosAngle");
	spotLight.cosFolloffStart = globalVariables->GetValue<float>(groupName, "cosFolloffStart");
	spotLight.decay = globalVariables->GetValue<float>(groupName, "decay");
	spotLight.direction = globalVariables->GetValue<Vector3>(groupName, "direction");
	spotLight.distance = globalVariables->GetValue<float>(groupName, "distance");
	spotLight.enableSpotLighting = globalVariables->GetValue<int32_t>(groupName, "enableSpotLighting");
	spotLight.intensity = globalVariables->GetValue<float>(groupName, "intensity");
	spotLight.isBlinnPhong = globalVariables->GetValue<int32_t>(groupName, "isBlinnPhong");
	spotLight.position = globalVariables->GetValue<Vector3>(groupName, "position");
}
