#include "Object3dCommon.h"
#include "engine/base/DirectXBase.h"
#include "engine/camera/Camera.h"
#include "engine/base/GraphicsPipeline.h"
#include "engine/debug/ImGuiManager.h"
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
	directionalLightData_.enableDirectionalLighting = true;

	//PointLightの初期化
	pointLightData_.color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData_.position = {};
	pointLightData_.intensity = 1.0f;
	pointLightData_.isBlinnPhong = false;
	pointLightData_.enablePointLighting = false;

	//SpotLightの初期化
	spotLightData_.color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData_.position = { 2.0f,1.25f,0.0f };
	spotLightData_.distance = 7.0f;
	spotLightData_.direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
	spotLightData_.intensity = 4.0f;
	spotLightData_.decay = 2.0f;
	spotLightData_.cosFolloffStart = 0.0f;
	spotLightData_.cosAngle = std::cos(Math::kPi / 3.0f);
	spotLightData_.isBlinnPhong = false;
	spotLightData_.enableSpotLighting = false;
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
	ImGui::ColorEdit4("point.color", &pointLightData_.color.x);
	ImGui::DragFloat3("point.position", &pointLightData_.position.x, 0.1f);
	ImGui::DragFloat("point.intensity", &pointLightData_.intensity, 0.1f);
	ImGuiManager::CheckBoxToInt("point.isBlingPhong", pointLightData_.isBlinnPhong);
	ImGuiManager::CheckBoxToInt("point.enablePointLight", pointLightData_.enablePointLighting);
	ImGui::ColorEdit4("spot.color", &spotLightData_.color.x);
	ImGui::DragFloat3("spot.position", &spotLightData_.position.x, 0.1f);
	ImGui::DragFloat3("spot.direction", &spotLightData_.direction.x, 0.1f);
	ImGui::SliderFloat("spot.cosFolloffStart", &spotLightData_.cosFolloffStart, -1.0f, 1.0f);
	ImGui::SliderFloat("spot.cosAngle", &spotLightData_.cosAngle, -1.0f, 1.0f);
	ImGui::DragFloat("spot.decay", &spotLightData_.decay, 0.1f);
	ImGui::DragFloat("spot.distance", &spotLightData_.distance, 0.1f);
	ImGui::DragFloat("spot.intensity", &spotLightData_.intensity, 0.1f);
	ImGuiManager::CheckBoxToInt("spot.isBlingPhong", spotLightData_.isBlinnPhong);
	ImGuiManager::CheckBoxToInt("spot.enableSpotLight", spotLightData_.enableSpotLighting);
	ImGui::End();
	*directionalLightPtr_ = directionalLightData_;
	*pointLightPtr_ = pointLightData_;
	*spotLightPtr_ = spotLightData_;
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
	//光源のリソースを作成
	pointLightResource_ = directXBase_->CreateBufferResource(sizeof(PointLight));
	//光源データの書きこみ
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightPtr_));
	pointLightPtr_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightPtr_->position = { 0.0f,-1.0f,0.0f };
	pointLightPtr_->intensity = 10.0f;
	pointLightPtr_->isBlinnPhong = true;
	pointLightPtr_->enablePointLighting = false;
}

//スポットライトの生成
void Object3dCommon::CreateSpotLight() {
	//光源のリソースを作成
	spotLightResource_ = directXBase_->CreateBufferResource(sizeof(SpotLight));
	//光源データの書きこみ
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightPtr_));
	spotLightPtr_->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightPtr_->position = { 2.0f,1.25f,0.0f };
	spotLightPtr_->distance = 7.0f;
	spotLightPtr_->direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
	spotLightPtr_->intensity = 4.0f;
	spotLightPtr_->decay = 2.0f;
	spotLightPtr_->cosAngle = std::cos(Math::kPi / 3.0f);
	spotLightPtr_->cosFolloffStart =0.0f;
	spotLightPtr_->isBlinnPhong = true;
	spotLightPtr_->enableSpotLighting = false;
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
