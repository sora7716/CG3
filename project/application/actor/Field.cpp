#include "Field.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "WireframeObject3d.h"

//コンストラクタ
Field::Field() {
}

//デストラクタ
Field::~Field() {
}

//初期化
void Field::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	//ゲームオブジェクトの初期化
	gameObject_.Initialize();
	gameObject_.transformData.translate = { 20.0f,0.0f,25.0f };
	//レンダーオブジェクトの初期化
	renderObject_ = renderObject_.Create().InitializeMaterial().Build();
	renderObject_.object3d->Initialize(object3dCommon, camera);
	renderObject_.object3d->SetModel(modelName_);
	renderObject_.hitBox->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kCube);
	//コライダーの状態
	colliderState_.Initialize();
	//コライダー
	collider_ = collider_
		.SetOwner(&colliderState_)
		.SetIsTrigger(false)
		.SetIsEnebled(false)
		.SetOnCollision([this](ColliderState* other) {this->OnCollision(other); })
		.Build();
}

//更新
void Field::Update() {
	renderObject_.object3d->Update();
}

//描画
void Field::Draw() {
	renderObject_.object3d->SetTransformData(0, gameObject_.transformData);
	renderObject_.object3d->Draw();
}

//衝突したとき
void Field::OnCollision(ColliderState* colliderState) {
	(void)colliderState;
}

//カメラのセッター
void Field::SetCamera(Camera* camera) {
	renderObject_.object3d->SetCamera(camera);
	renderObject_.hitBox->SetCamera(camera);
}
