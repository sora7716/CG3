#pragma once
#include "SpotLight.h"
#include <string>
#include<map>
#include <memory>

//前方宣言
class Camera;

/// <summary>
/// スポットライトの管理
/// </summary>
class SpotLightManager{
public://メンバ関数
	/// <summary>
	/// インスタンスのゲッター
	/// </summary>
	/// <returns>インスタンス</returns>
	static SpotLightManager* GetInstance();

	/// <summary>
    /// 終了
    /// </summary>
	void Finalize();

	/// <summary>
	/// スポットライトを追加
	/// </summary>
	/// <param name="name">名前</param>
	/// <param name="camera">カメラ</param>
	void AddSpotLight(const std::string& name,Camera*camera);

	/// <summary>
	/// スポットライトを検索
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>スポットライト</returns>
	SpotLight* FindSpotLight(const std::string& name);
private://メンバ関数
	//コンストラクタの封印
	SpotLightManager() = default;
	//デストラクタの封印
	~SpotLightManager() = default;
	//コピーコンストラクタ禁止
	SpotLightManager(const SpotLightManager&) = delete;
	//代入演算子の禁止
	SpotLightManager operator=(const SpotLightManager&) = delete;
private://静的メンバ変数
	//インスタンス
	static inline SpotLightManager* instance = nullptr;
	//Finalizeをしたかどうかのフラグ
	static inline bool isFinalize = false;
private://メンバ変数
	//スポットライトリスト
	std::map<std::string, std::unique_ptr<SpotLight>>spotLightList_;
};

