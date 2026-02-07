#pragma once
#include "engine/base/DirectXBase.h"
#include "engine/base/SRVManager.h"
#include "engine/base/BlendMode.h"
#include "engine/2d/Object2d.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/debug/DebugCamera.h"
#include "engine/audio/AudioManager.h"
#include "engine/2d/Sprite.h"
#include "engine/3d/Object3d.h"
#include "engine/particle/ParticleManager.h"
#include "engine/debug/WireframeObject3d.h"
#include <memory>

// 前方宣言
class Core;
class Input;
class SceneManager;
class AbstractSceneFactory;
class CameraManager;

/// <summary>
/// シーンのインターフェース
/// </summary>
class IScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IScene() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="core">ゲームエンジンの核</param>
	virtual void Initialize(Core*core);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	//純粋仮想関数
	virtual void Draw() = 0;
protected://メンバ変数
	//ゲームエンジンの核
	Core* core_ = nullptr;
	//デバックカメラ
	std::unique_ptr<DebugCamera>debugCamera_ = nullptr;
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
};

