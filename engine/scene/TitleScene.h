#pragma once
#include "IScene.h"
#include "engine/2d/Sprite.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene :public IScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TitleScene() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene()override = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	void Initialize(DirectXBase* directXBase)override;

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
	//3Dモデル
	std::unique_ptr<Object3d>object3des_[2] = { nullptr };
	TransformData worldTransform3d_[2] = {};
	Vector4 object3dColor_ = { 1.0f,1.0f,1.0f,1.0f };
	//ライト
	DirectionalLight directionalLight_ = {};
	//ブレンドモード
	int32_t blendMode_ = static_cast<int32_t>(BlendMode::kNone);
	std::unique_ptr<ParticleSystem>particleSystem_ = nullptr;

	Vector3 cameraRotate_ = {};

};
