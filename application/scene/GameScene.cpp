#include "GameScene.h"
#include "engine/input/Input.h"
#include "engine/3d/Object3dCommon.h"
#include "engine/debug/GlobalVariables.h"
#include "engine/math/func/Collision.h"
#include "engine/scene/SceneManager.h"
#include "engine/camera/CameraManager.h"
#include "engine/base/Core.h"
#include "actor/Bullet.h"

//初期化
void GameScene::Initialize(Core* core) {
	//シーンのインタフェースの初期化
	IScene::Initialize(core);
	//カメラの設定
	camera_ = core_->GetCameraManager()->FindCamera("gameCamera");

	//追従カメラ
	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize(camera_, { 0.0f,20.0f,-12.0f });

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(core_->GetInput(), core_->GetSpriteCommon(), core_->GetObject3dCommon(), gameCamera_->GetCamera(), "player");
	player_->SetPosition({ 25.0f,0.0f,25.0f });

	//フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize(core_->GetObject3dCommon(), gameCamera_->GetCamera());

	//敵の実装
	enemyManager_ = EnemyManager::GetInstance();
	enemyManager_->Initialize(core_->GetObject3dCommon(), camera_);

	//スコア
	score_ = std::make_unique<Score>();
	score_->Initialize(core_->GetObject2dCommon());
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

	//プレイヤー
	player_->Update();
	Vector3 playerPos = player_->GetTransformData().translate;
	core_->GetObject3dCommon()->SetPointLightPos({ playerPos.x,playerPos.y + 1.3f,playerPos.z });

	//敵
	enemyManager_->Update(player_->GetWorldPos());

	//フィールド
	field_->SetDirectionalLight(core_->GetObject3dCommon()->GetDirectionalLight());
	field_->SetPointLight(core_->GetObject3dCommon()->GetPointLight());
	field_->SetSpotLight(core_->GetObject3dCommon()->GetSpotLightPtr());
	field_->Update();

	//スコア
	score_->Update();

	//衝突判定
	//敵とプレイヤーの弾
	for (Enemy* enemy : enemyManager_->GetEnemies()) {
		if (!enemy->IsAlive()) {
			continue;//敵が生存していなかったら
		}
		for (uint32_t i = 0; i < player_->GetBullet()->GetBulletData().size(); i++) {
			if (player_->GetBullet()->GetBulletData()[i].gameObject.isAlive) {
				if (Collision::IsCollision(enemy->GetOBB(), player_->GetBullet()->GetBulletData()[i].gameObject.hitBox->GetOBB())) {
					player_->GetBullet()->OnCollision(i);
					enemy->OnCollision();
					break;
				}
			}
		}
	}

	//プレイヤーと敵の弾
	for (Enemy* enemy : enemyManager_->GetEnemies()) {
		for (uint32_t i = 0; i < enemy->GetBullet()->GetBulletData().size(); i++) {
			if (enemy->GetBullet()->GetBulletData()[i].gameObject.isAlive) {
				if (Collision::IsCollision(player_->GetOBB(), enemy->GetBullet()->GetBulletData()[i].gameObject.hitBox->GetOBB())) {
					enemy->GetBullet()->OnCollision(i);
					player_->OnCollision();
					break;
				}
			}
		}
	}

	//プレイヤーが死んだら
	if (!player_->IsAlive()) {
		core_->GetSceneManager()->ChangeScene("Result");
	}

#ifdef USE_IMGUI
	//ImGuiの受付開始
	core_->GetImGuiManager()->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	//グローバル変数の更新
	GlobalVariables::GetInstance()->Update();

	//プレイヤー
	ImGui::Begin("player");
	player_->Debug();
	ImGui::End();

	//レイルカメラ
	ImGui::Begin("gameCamera");
	gameCamera_->Debug();
	ImGui::End();

	//敵
	ImGui::Begin("enemy");
	enemyManager_->Debug();
	ImGui::End();

	//スコア
	//ImGui::Text("score:%d", Score::score);

	//Object3dCommon
	core_->GetObject3dCommon()->Debug();

	//ImGuiの受付終了
	core_->GetImGuiManager()->End();
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
	enemyManager_->Draw();

	//スコア
	score_->Draw();
}

//終了
void GameScene::Finalize() {
	//敵の解放
	EnemyManager::GetInstance()->Finalize();
	//シーンのインターフェース
	IScene::Finalize();
}
