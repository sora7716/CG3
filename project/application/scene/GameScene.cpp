#define NOMINMAX
#include "GameScene.h"
#include "engine/input/Input.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/debug/GlobalVariables.h"
#include "engine/math/func/Collision.h"
#include "engine/scene/SceneManager.h"
#include "engine/camera/CameraManager.h"
#include "engine/debug/WireframeObject3d.h"
#include "engine/base/Core.h"
#include "actor/Bullet.h"
#include "actor/Player.h"
#include "actor/GameCamera.h"
#include "actor/Field.h"
#include "actor/EnemyManager.h"
#include "actor/Score.h"
#include "engine/debug/WireframeObject3d.h"
#include "engine/math/func/Math.h"
#include "engine/math/func/Physics.h"

//コンストラクタ
GameScene::GameScene() {
}

//デストラクタ
GameScene::~GameScene() {
}

//初期化
void GameScene::Initialize(const SceneContext& sceneContext) {
	//シーンのインタフェースの初期化
	IScene::Initialize(sceneContext);
	//カメラの設定
	camera_ = sceneContext_.cameraManager->FindCamera("gameCamera");

	//追従カメラ
	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize(camera_);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(sceneContext_.input, sceneContext_.spriteCommon, sceneContext_.object3dCommon, gameCamera_->GetCamera(), "player");
	player_->SetPosition({ 25.0f,0.0f,25.0f });

	//フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize(sceneContext_.object3dCommon, gameCamera_->GetCamera());

	//敵の実装
	enemyManager_ = EnemyManager::GetInstance();
	enemyManager_->Initialize(sceneContext_.object3dCommon, camera_);

	//スコア
	score_ = std::make_unique<Score>();
	score_->Initialize(sceneContext_.object2dCommon);

	//ワイヤーモデル
	wireframeObject3d_ = std::make_unique<WireframeObject3d>();
	wireframeObject3d_->Initialize(sceneContext_.wireframeObject3dCommon, camera_, ModelType::kCube);
	wireframeTransformDate_.scale = Vector3::MakeAllOne();
	wireframeTransformDate_.translate = { 20.0f,0.0f,25.0f };

	sphere_ = std::make_unique<WireframeObject3d>();
	sphere_->Initialize(sceneContext_.wireframeObject3dCommon, camera_, ModelType::kSphere);
	ball_.position = { 0.8f,0.2f,0.0f };
	ball_.acceleration = { 0.0f,Physics::kGravity,0.0f };
	ball_.mass = 2.0f;
	ball_.radius = 0.05f;

	spring_.anchor = { 0.0f,1.0f,0.0f };
	spring_.naturalLength = 0.7f;
	spring_.stiffness = 100.0f;
	spring_.dampingCoefficient = 2.0f;

	pendulum_.anchor = { 0.0f,1.0f,0.0f };
	pendulum_.length = 0.8f;
	pendulum_.angle = 0.7f;
	pendulum_.angularAcceleration = 0.0f;
	pendulum_.angularVelocity = 0.0f;

	anchorPoint_ = std::make_unique<WireframeObject3d>();
	anchorPoint_->Initialize(sceneContext_.wireframeObject3dCommon, camera_, ModelType::kSphere);
}

//更新
void GameScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	//追従カメラ
	gameCamera_->Update();
	gameCamera_->SetTragetPos(player_->GetTransformData().translate);

	//カメラの設定
	player_->SetCamera(camera_);
	field_->SetCamera(camera_);
	enemyManager_->SetCamera(camera_);
	wireframeObject3d_->SetCamera(camera_);
	sphere_->SetCamera(camera_);
	anchorPoint_->SetCamera(camera_);

	Vector3 prePlayerPos = player_->GetTransformData().translate;

	//プレイヤー
	player_->Update();
	Vector3 playerPos = player_->GetTransformData().translate;
	sceneContext_.object3dCommon->SetPointLightPos({ playerPos.x,playerPos.y + 2.0f,playerPos.z });

	//敵
	//enemyManager_->Update(player_->GetWorldPos());

	//フィールド
	field_->SetDirectionalLight(sceneContext_.object3dCommon->GetDirectionalLight());
	field_->SetPointLight(sceneContext_.object3dCommon->GetPointLight());
	field_->SetSpotLight(sceneContext_.object3dCommon->GetSpotLightPtr());
	field_->Update();

	//スコア
	score_->Update();

	//ワイヤーフレームモデル
	wireframeObject3d_->SetScale(wireframeTransformDate_.scale);
	wireframeObject3d_->SetRotate(wireframeTransformDate_.rotate);
	wireframeObject3d_->SetTranslate(wireframeTransformDate_.translate);
	wireframeObject3d_->Update();

	sphere_->SetTranslate(ball_.position);
	sphere_->SetRadius(ball_.radius);
	//下に行き過ぎないように制限
	//ball_.position.y = std::max(ball_.position.y, 0.0f);	
	//ball_.acceleration = Physics::ApplySpringForce(spring_, ball_);
	//ball_.acceleration.y += Physics::kGravity;
	//ball_.velocity += ball_.acceleration * Math::kDeltaTime;
	//ball_.position += ball_.velocity * Math::kDeltaTime;
	ball_.position = Physics::ApplyPendulumForce(pendulum_, ball_.position);
	sphere_->Update();

	anchorPoint_->SetTranslate(pendulum_.anchor);
	//anchorPoint_->SetTranslate(spring_.anchor);
	anchorPoint_->SetRadius(0.05f);
	anchorPoint_->Update();

	//ワイヤーフレームモデルとプレイヤーの当たり判定
	if (Collision::IsCollision(player_->GetOBB(), wireframeObject3d_->GetOBB())) {
		wireframeObject3d_->SetColor(Vector4::MakeRedColor());

		Vector3 pos = player_->GetWorldPos();
		Vector3 vel = player_->GetVelocity();

		float blockRight = wireframeObject3d_->GetOBB().center.x + wireframeObject3d_->GetOBB().size.x;
		float blockTop = wireframeObject3d_->GetOBB().center.y + wireframeObject3d_->GetOBB().size.y;
		float playerLeft = pos.x - player_->GetOBB().size.x;
		float playerBottom = pos.y - player_->GetOBB().size.y;
		if (playerBottom < blockTop) {
			player_->SetIsOnGround(true);
			pos.y = std::max(pos.y, prePlayerPos.y);
			vel.y = 0.0f;
		} else {
			player_->SetIsOnGround(false);
		}

		if (playerLeft < blockRight) {
			if (playerBottom + 0.05f < blockTop) {
				pos.x = prePlayerPos.x;
			}
		}

		float blockFront = wireframeObject3d_->GetOBB().center.z + wireframeObject3d_->GetOBB().size.z;
		float blockBehind = wireframeObject3d_->GetOBB().center.z - wireframeObject3d_->GetOBB().size.z;
		float playerFront = player_->GetOBB().center.z + player_->GetOBB().size.z;
		float playerBehind = player_->GetOBB().center.z - player_->GetOBB().size.z;

		if (playerBehind < blockFront) {
			if (playerBottom + 0.05f < blockTop) {
				pos.z = prePlayerPos.z;
			}
		}

		player_->SetPosition(pos);
		player_->SetVelocity(vel);
	} else {
		wireframeObject3d_->SetColor(Vector4::MakeBlackColor());
	}
	////衝突判定
	////敵とプレイヤーの弾
	//for (Enemy* enemy : enemyManager_->GetEnemies()) {
	//	if (!enemy->IsAlive()) {
	//		continue;//敵が生存していなかったら
	//	}
	//	for (uint32_t i = 0; i < player_->GetBullet()->GetBulletData().size(); i++) {
	//		if (player_->GetBullet()->GetBulletData()[i].gameObject.isAlive) {
	//			if (Collision::IsCollision(enemy->GetOBB(), player_->GetBullet()->GetBulletData()[i].gameObject.hitBox->GetOBB())) {
	//				player_->GetBullet()->OnCollision(i);
	//				enemy->OnCollision();
	//				break;
	//			}
	//		}
	//	}
	//}

	////プレイヤーと敵の弾
	//for (Enemy* enemy : enemyManager_->GetEnemies()) {
	//	for (uint32_t i = 0; i < enemy->GetBullet()->GetBulletData().size(); i++) {
	//		if (enemy->GetBullet()->GetBulletData()[i].gameObject.isAlive) {
	//			if (Collision::IsCollision(player_->GetOBB(), enemy->GetBullet()->GetBulletData()[i].gameObject.hitBox->GetOBB())) {
	//				enemy->GetBullet()->OnCollision(i);
	//				player_->OnCollision();
	//				break;
	//			}
	//		}
	//	}
	//}

	//プレイヤーが死んだら
	if (!player_->IsAlive()) {
		sceneContext_.sceneManager->ChangeScene("Result");
	}

#ifdef USE_IMGUI
	//ImGuiの受付開始
	sceneContext_.imguiManager->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	//グローバル変数の更新
	//GlobalVariables::GetInstance()->Update();

	//プレイヤー
	ImGui::Begin("player");
	player_->Debug();
	ImGui::End();

	//ゲームカメラ
	ImGui::Begin("gameCamera");
	gameCamera_->Debug();
	ImGui::End();

	//ワイヤーモデル
	ImGui::Begin("wireframe");
	sceneContext_.imguiManager->DragTransform(wireframeTransformDate_);
	ImGui::End();

	ImGui::Begin("physics");
	ImGui::DragFloat3("spring.anchor", &spring_.anchor.x, 0.1f);
	ImGui::DragFloat("spring.naturalLength", &spring_.naturalLength, 0.1f);
	ImGui::DragFloat("spring.stiffness", &spring_.stiffness, 0.1f);
	ImGui::DragFloat("dampingCoefficient", &spring_.dampingCoefficient, 0.1f);
	ImGui::DragFloat3("pendulum.anchor", &pendulum_.anchor.x, 0.1f);
	ImGui::DragFloat("pendulum.length", &pendulum_.length, 0.1f);
	ImGui::DragFloat3("ball.acceleration", &ball_.acceleration.x, 0.1f);
	ImGui::DragFloat3("ball.velocity", &ball_.velocity.x, 0.1f);
	ImGui::DragFloat3("ball.position", &ball_.position.x, 0.1f);
	ImGui::End();

	//敵
	//ImGui::Begin("enemy");
	//enemyManager_->Debug();
	//ImGui::End();

	//スコア
	//ImGui::Text("score:%d", Score::score);

	//Object3dCommon
	//sceneContext_.object3dCommon->Debug();

	//ImGuiの受付終了
	sceneContext_.imguiManager->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	//カメラの切り替え
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = gameCamera_->GetCamera();
	}
#endif // _DEBUG

}

//描画
void GameScene::Draw() {
	//プレイヤー
	player_->Draw();

	//マップチップ
	field_->Draw();

	//敵
	//enemyManager_->Draw();

	//スコア
	score_->Draw();

	//ワイヤフレームモデル
	wireframeObject3d_->Draw();
	sphere_->Draw();
	anchorPoint_->Draw();
}

//終了
void GameScene::Finalize() {
	//敵の解放
	EnemyManager::GetInstance()->Finalize();
	//シーンのインターフェース
	IScene::Finalize();
}
