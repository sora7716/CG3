#include "Ground.h"
#include "engine/camera/Camera.h"
#include "engine/3d/Object3d.h"
#include "engine/debug/ImGuiManager.h"
#include <cmath>

//初期化
void Ground::Initialize(Camera* camera, const std::string& modelName) {
	//地面の初期化
	groundDates_.resize(kBlockCount.y);
	for (int32_t i = 0; i < kBlockCount.y; i++) {
		groundDates_[i].resize(kBlockCount.x);
		for (int32_t j = 0; j < kBlockCount.x; j++) {
			//3Dモデル
			groundDates_[i][j].object = new Object3d();
			groundDates_[i][j].object->Initialize(camera);
			groundDates_[i][j].object->SetModel(modelName);

			//カメラの設定
			groundDates_[i][j].object->SetCamera(camera);

			//トランスフォームデータの初期化
			groundDates_[i][j].transform.scale = Vector3::MakeAllOne();
			groundDates_[i][j].transform.rotate = {};
			groundDates_[i][j].transform.translate.y = -2.0f;
			groundDates_[i][j].transform.translate.x = (groundDates_[i][j].transform.scale.x + 1.0f) * i; // x座標の計算
			groundDates_[i][j].transform.translate.z = (groundDates_[i][j].transform.scale.z + 1.0f) * j; // z座標の計算
		}
	}

	//マテリアルの初期化
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.shininess = 10.0f;
	material_.uvMatrix = Matrix4x4::Identity4x4();
}

//更新
void Ground::Update() {
	for (std::vector<GroundData>& groundBlockLine : groundDates_) {
		for (GroundData& groundBlock : groundBlockLine) {
			//トランスフォームデータのセット
			groundBlock.object->SetTransform(groundBlock.transform);

			//マテリアルのセット
			groundBlock.object->GetModel()->SetMaterial(material_);

			//3Dオブジェクトの更新
			groundBlock.object->Update();
		}
	}
}

//描画
void Ground::Draw() {
	for (std::vector<GroundData>& groundBlockLine : groundDates_) {
		for (GroundData& groundBlock : groundBlockLine) {
			//3Dオブジェクトの描画
			groundBlock.object->Draw();
		}
	}
}

//終了
void Ground::Finalize() {
	//3Dオブジェクトを解放
	for (std::vector<GroundData>& groundBlockLine : groundDates_) {
		for (GroundData& groundBlock : groundBlockLine) {
			delete groundBlock.object;
			groundBlock.object = nullptr;
		}
	}
	groundDates_.clear();
}

//デバッグ
void Ground::Debug() {
#ifdef USE_IMGUI
	ImGui::ColorEdit4("color", &material_.color.x);
	ImGui::DragFloat("shininess", &material_.shininess, 0.1f);
	ImGuiManager::CheckBoxToInt("enableLighting", material_.enableLighting);
#endif // USE_IMGUI
}

//カメラのセッター
void Ground::SetCamera(Camera* camera) {
	for (std::vector<GroundData>& groundBlockLine : groundDates_) {
		for (GroundData& groundBlock : groundBlockLine) {
			groundBlock.object->SetCamera(camera);
		}
	}
}

//テクスチャのセッター
void Ground::SetTexture(const std::string& textureName) {
	for (std::vector<GroundData>& groundBlockLine : groundDates_) {
		for (GroundData& groundBlock : groundBlockLine) {
			groundBlock.object->SetTexture(textureName);
		}
	}
}