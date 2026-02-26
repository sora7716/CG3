#pragma once
#include "engine/scene/IScene.h"
#include "engine/math/RenderingData.h"
#include "engine/math/CollisionPrimitives.h"
#include "engine/math/PhysicsData.h"

//前方宣言
class Player;
class GameCamera;
class Field;
class EnemyManager;
class Score;
class WireframeObject3d;

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene :public IScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene()override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sceneContext">シーンで必要なもの</param>
	void Initialize(const SceneContext& sceneContext)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://メンバ変数
	//ゲームシーンのカメラ
	Camera* camera_ = nullptr;

	//プレイヤー
	std::unique_ptr<Player>player_ = nullptr;

	//レールカメラ
	std::unique_ptr<GameCamera>gameCamera_ = nullptr;

	//フィールド
	std::unique_ptr<Field>field_ = nullptr;

	//敵の管理
	EnemyManager* enemyManager_ = nullptr;

	//スコアの表示
	std::unique_ptr<Score>score_ = nullptr;

	//ワイヤーモデル
	std::unique_ptr<WireframeObject3d>wireframeObject3d_ = nullptr;
	TransformData wireframeTransformDate_ = {};
};

