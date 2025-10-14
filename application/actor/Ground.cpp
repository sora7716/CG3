#include "Ground.h"
#include "engine/camera/Camera.h"
#include "engine/3d/Object3d.h"

//初期化
void Ground::Initialize(Camera* camera,const std::string& modelName) {
	//3Dモデル
	groundData_.object = new Object3d();
	groundData_.object->Initialize();
	groundData_.object->SetModel(modelName);

	//カメラの設定
	groundData_.object->SetCamera(camera);

	//トランスフォームデータの初期化
	groundData_.transform.scale = Vector3::MakeAllOne();
	groundData_.transform.rotate = {};
	groundData_.transform.translate = {};
}

//更新
void Ground::Update() {
	//トランスフォームデータのセット
	groundData_.object->SetTransform(groundData_.transform);

	//3Dオブジェクトの更新
	groundData_.object->Update();
}

//描画
void Ground::Draw() {
	//3Dオブジェクトの描画
	groundData_.object->Draw();
}


//終了
void Ground::Finalize() {
	//3Dオブジェクトを解放
	delete groundData_.object;
	groundData_.object = nullptr;
}

//カメラのセッター
void Ground::SetCamera(Camera* camera) {
	//カメラの設定
	groundData_.object->SetCamera(camera);
}

//テクスチャのセッター
void Ground::SetTexture(const std::string& textureName) {
	groundData_.object->SetTexture(textureName);
}