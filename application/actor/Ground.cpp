#include "Ground.h"
#include "engine/camera/Camera.h"
#include "engine/3d/Object3d.h"

//初期化
void Ground::Initialize(Camera* camera, const std::string& modelName) {
	groundDates_.resize(kBlockCount.y);
	for (int32_t i = 0; i < kBlockCount.y; i++) {
		groundDates_[i].resize(kBlockCount.x);
		for (int32_t j = 0; j < kBlockCount.x; j++) {
			//3Dモデル
			groundDates_[i][j].object = new Object3d();
			groundDates_[i][j].object->Initialize();
			groundDates_[i][j].object->SetModel(modelName);

			//カメラの設定
			groundDates_[i][j].object->SetCamera(camera);

			//トランスフォームデータの初期化
			groundDates_[i][j].transform.scale = Vector3::MakeAllOne();
			groundDates_[i][j].transform.rotate = {};
			groundDates_[i][j].transform.translate = { 2.0f * j,-2.0f,2.0f * i };
		}
	}
}

//更新
void Ground::Update() {
	for (std::vector<GroundData>& groundBlockLine : groundDates_) {
		for (GroundData& groundBlock : groundBlockLine) {
			//トランスフォームデータのセット
			groundBlock.object->SetTransform(groundBlock.transform);

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