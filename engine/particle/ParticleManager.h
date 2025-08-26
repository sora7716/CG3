#pragma once
#include "ParticleSystem.h"
#include <map>
#include <memory>
#include <string>

/// <summary>
/// パーティクルの管理クラス
/// </summary>
class ParticleManager {
public://メンバ関数
	/// <summary>
	/// インスタンスのゲッター
	/// </summary>
	/// <returns></returns>
	static ParticleManager* GetInstance();

	/// <summary>
	/// パーティクルシステムの追加
	/// </summary>
	/// <param name="name">名前</param>
	void AddParticleSystem(const std::string& name);

	/// <summary>
	/// パーティクルシステムを検索
	/// </summary>
	/// <param name="name">名前</param>
	ParticleSystem* FindParticleSystem(const std::string& name);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	//コピーコンストラクタ禁止
	ParticleManager(const ParticleManager&) = delete;
	//代入演算子を禁止
	ParticleManager operator=(const ParticleManager&) = delete;
private://メンバ関数
	//コンストラクタの封印
	ParticleManager() = default;
	//デストラクタの封印
	~ParticleManager() = default;
public://静的メンバ変数
	//インスタンス
	static inline ParticleManager* instance = nullptr;
	//終了処理をしたかのフラグ
	static inline bool isFinalize = false;
private://メンバ変数
	//パーティクルシステムのマップ
	std::map<std::string, std::unique_ptr<ParticleSystem>> particleSystems_;
};

