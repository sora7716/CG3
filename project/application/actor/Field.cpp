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
	//壁の数
	wallGroup_.wallCount = 2;
	wallGroup_.wallDescs.resize(wallGroup_.wallCount);
	wallGroup_.modelName = "wall";
	//レンダーオブジェクトの初期化
	wallGroup_.renderObject = wallGroup_.renderObject
		.Create()
		.InitializeMaterial()
		.Build();
	wallGroup_.renderObject.object3d->Initialize(object3dCommon, camera, wallGroup_.wallCount);
	wallGroup_.renderObject.object3d->SetModel(wallGroup_.modelName);
	wallGroup_.renderObject.hitBox->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kCube, wallGroup_.wallCount);

	//WallDescの初期化
	for (int32_t i = 0; i < wallGroup_.wallCount; i++) {
		//ヒットボックスの大きさを設定
		wallGroup_.wallDescs[i].hitBoxScale = Vector3::MakeAllOne();
		//ゲームオブジェクトの初期化
		wallGroup_.wallDescs[i].gameObject.Initialize();

		//コライダーの状態の初期化
		wallGroup_.wallDescs[i].colliderState.Initialize();
		wallGroup_.wallDescs[i].colliderState.scalePtr = &wallGroup_.wallDescs[i].gameObject.transformData.scale;
		wallGroup_.wallDescs[i].colliderState.rotatePtr = &wallGroup_.wallDescs[i].gameObject.transformData.rotate;
		wallGroup_.wallDescs[i].colliderState.translatePtr = &wallGroup_.wallDescs[i].gameObject.transformData.translate;
		wallGroup_.wallDescs[i].colliderState.velocityPtr = &wallGroup_.wallDescs[i].gameObject.velocity;
		wallGroup_.wallDescs[i].colliderState.worldMatrixPtr = &wallGroup_.renderObject.object3d->GetWorldMatrix(i);
		wallGroup_.wallDescs[i].colliderState.isOnGroundPtr = &wallGroup_.wallDescs[i].gameObject.isOnGround;

		//コライダーの初期化
		wallGroup_.wallDescs[i].collider = wallGroup_.wallDescs[i].collider
			.SetOwner(&wallGroup_.wallDescs[i].colliderState)
			.SetIsTrigger(false)
			.SetIsEnebled(true)
			.SetOnCollision([this](ColliderState* other) {this->OnCollision(other); })
			.Build();
	}

	//壁の位置を指定
	wallGroup_.wallDescs[0].gameObject.transformData.translate = { 20.0f,0.0f,25.0f };
	wallGroup_.wallDescs[1].gameObject.transformData.translate = { 18.0f,0.0f,25.0f };
}

//更新
void Field::Update() {
	for (int32_t i = 0; i < wallGroup_.wallCount; i++) {
		wallGroup_.renderObject.object3d->SetTransformData(i, wallGroup_.wallDescs[i].gameObject.transformData);
		wallGroup_.renderObject.hitBox->SetScale(i, wallGroup_.wallDescs[i].hitBoxScale);
		wallGroup_.renderObject.hitBox->SetRotate(i, wallGroup_.wallDescs[i].gameObject.transformData.rotate);
		wallGroup_.renderObject.hitBox->SetTranslate(i, wallGroup_.renderObject.object3d->GetWorldPos(i));
	}
	wallGroup_.renderObject.object3d->Update();
	wallGroup_.renderObject.hitBox->Update();
}

//描画
void Field::Draw() {
	//wallGroup_.renderObject.object3d->Draw();
	wallGroup_.renderObject.hitBox->Draw();
}

//衝突したとき
void Field::OnCollision(ColliderState* colliderState) {
	(void)colliderState;
}

//カメラのセッター
void Field::SetCamera(Camera* camera) {
	wallGroup_.renderObject.object3d->SetCamera(camera);
	wallGroup_.renderObject.hitBox->SetCamera(camera);
}

//コライダーのゲッター
std::vector<WallDesc>& Field::GetWallDescs() {
	return wallGroup_.wallDescs;
}
