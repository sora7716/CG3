#include "Object3dCommon.h"
#include "engine/base/DirectXBase.h"
#include "engine/camera/Camera.h"
#include "engine/base/GraphicsPipeline.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/base/SRVManager.h"
#include "engine/2d/TextureManager.h"
#include <cassert>
#include "engine/math/func/Math.h"
using namespace Microsoft::WRL;

//インスタンスのゲッター
Object3dCommon* Object3dCommon::GetInstance() {
	assert(!isFinalize && "GetInstance() called after Finalize()");
	if (instance == nullptr) {
		instance = new Object3dCommon();
	}
	return instance;
}

//初期化
void Object3dCommon::Initialize(DirectXBase* directXBase) {
	assert(directXBase);//Nullチェック
	directXBase_ = directXBase;//DirectXの基盤を受け取る
	//ブレンド
	blend_ = new Blend();
	//グラフィックスパイプラインの生成と初期化
	makeGraphicsPipeline_ = new GraphicsPipeline();
	//シェーダを設定
	makeGraphicsPipeline_->SetVertexShaderFileName(L"Object3d.VS.hlsl");
	makeGraphicsPipeline_->SetPixelShaderFileName(L"Object3d.PS.hlsl");
	//デプスステンシルステート
	directXBase_->InitializeDepthStencilForObject3d();
	makeGraphicsPipeline_->Initialize(directXBase_);
	//ルートシグネイチャの記録
	rootSignature_ = makeGraphicsPipeline_->GetRootSignature();
	//グラフィックスパイプラインステートの記録
	graphicsPipelineStates_ = makeGraphicsPipeline_->GetGraphicsPipelines();

	//DirectionalLightの初期化
	directionalLightData_.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_.direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_.intensity = 1.0f;
	directionalLightData_.isLambert = false;
	directionalLightData_.isBlinnPhong = true;
	directionalLightData_.enableDirectionalLighting = false;

	//PointLightの初期化
	for (int i = 0; i < kMaxLightCount; i++) {
		pointLightDatas_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		pointLightDatas_[i].position = {};
		pointLightDatas_[i].intensity = 1.0f;
		pointLightDatas_[i].distance = 7.0f;
		pointLightDatas_[i].decay = 4.0f;
		pointLightDatas_[i].isBlinnPhong = false;
		pointLightDatas_[i].enablePointLighting = false;
	}

	//SpotLightの初期化
	for (int i = 0; i < kMaxLightCount; i++) {
		spotLightDatas_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		spotLightDatas_[i].position = { 2.0f,1.25f,0.0f };
		spotLightDatas_[i].distance = 7.0f;
		spotLightDatas_[i].direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
		spotLightDatas_[i].intensity = 4.0f;
		spotLightDatas_[i].decay = 2.0f;
		spotLightDatas_[i].cosFolloffStart = 1.0f;
		spotLightDatas_[i].cosAngle = std::cos(Math::kPi / 3.0f);
		spotLightDatas_[i].isBlinnPhong = false;
		spotLightDatas_[i].enableSpotLighting = false;
	}

	//ライティング
	//平行光源の生成
	CreateDirectionLight();
	//点光源の生成
	CreatePointLight();
	CreateStructuredBufferForPoint();
	//スポットライトの生成
	CreateSpotLight();
	CreateStructuredBufferForSpot();
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
	ImGui::Begin("Lighting");
	ImGui::ColorEdit4("directional.color", &directionalLightData_.color.x);
	ImGui::DragFloat3("directional.direction", &directionalLightData_.direction.x, 0.1f);
	ImGui::DragFloat("directional.intensity", &directionalLightData_.intensity, 0.1f);
	ImGuiManager::CheckBoxToInt("directional.isLambert", directionalLightData_.isLambert);
	ImGuiManager::CheckBoxToInt("directional.isBlingPhong", directionalLightData_.isBlinnPhong);
	ImGuiManager::CheckBoxToInt("directional.enableDirectionalLight", directionalLightData_.enableDirectionalLighting);
	ImGui::End();

	ImGui::Begin("point");
	ImGui::ColorEdit4("0.color", &pointLightDatas_[0].color.x);
	ImGui::DragFloat3("0.position", &pointLightDatas_[0].position.x, 0.1f);
	ImGui::DragFloat("0.intensity", &pointLightDatas_[0].intensity, 0.1f);
	ImGui::DragFloat("0.distance", &pointLightDatas_[0].distance, 0.1f);
	ImGui::DragFloat("0.decay", &pointLightDatas_[0].decay, 0.1f);
	ImGuiManager::CheckBoxToInt("0.isBlingPhong", pointLightDatas_[0].isBlinnPhong);
	ImGuiManager::CheckBoxToInt("0.enablePointLight", pointLightDatas_[0].enablePointLighting);
	ImGui::ColorEdit4("1.color", &pointLightDatas_[1].color.x);
	ImGui::DragFloat3("1.position", &pointLightDatas_[1].position.x, 0.1f);
	ImGui::DragFloat("1.intensity", &pointLightDatas_[1].intensity, 0.1f);
	ImGui::DragFloat("1.distance", &pointLightDatas_[1].distance, 0.1f);
	ImGui::DragFloat("1.decay", &pointLightDatas_[1].decay, 0.1f);
	ImGuiManager::CheckBoxToInt("1.isBlingPhong", pointLightDatas_[1].isBlinnPhong);
	ImGuiManager::CheckBoxToInt("1.enablePointLight", pointLightDatas_[1].enablePointLighting);
	ImGui::ColorEdit4("2.color", &pointLightDatas_[2].color.x);
	ImGui::DragFloat3("2.position", &pointLightDatas_[2].position.x, 0.1f);
	ImGui::DragFloat("2.intensity", &pointLightDatas_[2].intensity, 0.1f);
	ImGui::DragFloat("2.distance", &pointLightDatas_[2].distance, 0.1f);
	ImGui::DragFloat("2.decay", &pointLightDatas_[2].decay, 0.1f);
	ImGuiManager::CheckBoxToInt("2.isBlingPhong", pointLightDatas_[2].isBlinnPhong);
	ImGuiManager::CheckBoxToInt("2.enablePointLight", pointLightDatas_[2].enablePointLighting);
	ImGui::End();

	ImGui::Begin("spot");
	ImGui::ColorEdit4("0.color", &spotLightDatas_[0].color.x);
	ImGui::DragFloat3("0.position", &spotLightDatas_[0].position.x, 0.1f);
	ImGui::DragFloat3("0.direction", &spotLightDatas_[0].direction.x, 0.1f);
	ImGui::DragFloat("0.cosFolloffStart", &spotLightDatas_[0].cosFolloffStart, 0.1f);
	ImGui::DragFloat("0.cosAngle", &spotLightDatas_[0].cosAngle, 0.1f);
	ImGui::DragFloat("0.decay", &spotLightDatas_[0].decay, 0.1f);
	ImGui::DragFloat("0.distance", &spotLightDatas_[0].distance, 0.1f);
	ImGui::DragFloat("0.intensity", &spotLightDatas_[0].intensity, 0.1f);
	ImGuiManager::CheckBoxToInt("0.isBlingPhong", spotLightDatas_[0].isBlinnPhong);
	ImGuiManager::CheckBoxToInt("0.enableSpotLight", spotLightDatas_[0].enableSpotLighting);
	ImGui::ColorEdit4("1.color", &spotLightDatas_[1].color.x);
	ImGui::DragFloat3("1.position", &spotLightDatas_[1].position.x, 0.1f);
	ImGui::DragFloat3("1.direction", &spotLightDatas_[1].direction.x, 0.1f);
	ImGui::DragFloat("1.cosFolloffStart", &spotLightDatas_[1].cosFolloffStart, 0.1f);
	ImGui::DragFloat("1.cosAngle", &spotLightDatas_[1].cosAngle, 0.1f);
	ImGui::DragFloat("1.decay", &spotLightDatas_[1].decay, 0.1f);
	ImGui::DragFloat("1.distance", &spotLightDatas_[1].distance, 0.1f);
	ImGui::DragFloat("1.intensity", &spotLightDatas_[1].intensity, 0.1f);
	ImGuiManager::CheckBoxToInt("1.isBlingPhong", spotLightDatas_[1].isBlinnPhong);
	ImGuiManager::CheckBoxToInt("1.enableSpotLight", spotLightDatas_[1].enableSpotLighting);
	ImGui::ColorEdit4("2.color", &spotLightDatas_[2].color.x);
	ImGui::DragFloat3("2.position", &spotLightDatas_[2].position.x, 0.1f);
	ImGui::DragFloat3("2.direction", &spotLightDatas_[2].direction.x, 0.1f);
	ImGui::DragFloat("2.cosFolloffStart", &spotLightDatas_[2].cosFolloffStart, 0.1f);
	ImGui::DragFloat("2.cosAngle", &spotLightDatas_[2].cosAngle, 0.1f);
	ImGui::DragFloat("2.decay", &spotLightDatas_[2].decay, 0.1f);
	ImGui::DragFloat("2.distance", &spotLightDatas_[2].distance, 0.1f);
	ImGui::DragFloat("2.intensity", &spotLightDatas_[2].intensity, 0.1f);
	ImGuiManager::CheckBoxToInt("2.isBlingPhong", spotLightDatas_[2].isBlinnPhong);
	ImGuiManager::CheckBoxToInt("2.enableSpotLight", spotLightDatas_[2].enableSpotLighting);
	ImGui::End();
	*directionalLightPtr_ = directionalLightData_;
	for (int i = 0; i < kMaxLightCount; i++) {
		pointLightPtr_[i] = pointLightDatas_[i];
		spotLightPtr_[i] = spotLightDatas_[i];
	}
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
	srvIndexPoint_ = SRVManager::GetInstance()->Allocate() + TextureManager::kSRVIndexTop;
	SRVManager::GetInstance()->CreateSRVForStructuredBuffer(
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
		spotLightPtr_[i].cosFolloffStart = 0.0f;
		spotLightPtr_[i].isBlinnPhong = true;
		spotLightPtr_[i].enableSpotLighting = false;
	}
}

//スポットライトのストラクチャバッファの生成
void Object3dCommon::CreateStructuredBufferForSpot() {
	//ストラクチャバッファを生成
	srvIndexSpot_ = SRVManager::GetInstance()->Allocate() + TextureManager::kSRVIndexTop;
	SRVManager::GetInstance()->CreateSRVForStructuredBuffer(
		srvIndexSpot_,
		spotLightResource_.Get(),
		kMaxLightCount,
		sizeof(SpotLight)
	);
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
ID3D12Resource* Object3dCommon::GetSpotLight() const {
	return spotLightResource_.Get();
}

//DirectXの基盤のゲッター
DirectXBase* Object3dCommon::GetDirectXBase() const {
	return directXBase_;
}

//グラフィックパイプラインのゲッター
std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> Object3dCommon::GetGraphicsPipelineStates() const {
	return graphicsPipelineStates_;
}

//終了
void Object3dCommon::Finalize() {
	delete blend_;
	delete makeGraphicsPipeline_;
	delete instance;
	instance = nullptr;
	isFinalize = true;
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
