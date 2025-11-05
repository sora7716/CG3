#pragma once
#include "engine/math/Vector3.h"
#include <variant>
#include <cstdint>
#include <string>
#include <map>

//項目
struct Item {
	//項目の値
	std::variant<int32_t, float, Vector3>value;
};

//グループ
struct Group {
	std::map<std::string, Item>items;
};

/// <summary>
/// グローバル変数
/// </summary>
class GlobalVariables {
public://メンバ関数
	/// <summary>
	/// インスタンスのゲッター
	/// </summary>
	/// <returns></returns>
	static GlobalVariables* GetInstance();

	/// <summary>
	/// グループの生成
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 値のセット(int32_t)
	/// </summary>
	/// <param name="groupName">グループ名</param>
	/// <param name="key">検索キー</param>
	/// <param name="value">値</param>
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	
	/// <summary>
	/// 値のセット(float)
	/// </summary>
	/// <param name="groupName">グループ名</param>
	/// <param name="key">検索キー</param>
	/// <param name="value">値</param>
	void SetValue(const std::string& groupName, const std::string& key, float value);
	
	/// <summary>
	/// 値のセット(Vector3)
	/// </summary>
	/// <param name="groupName">グループ名</param>
	/// <param name="key">検索キー</param>
	/// <param name="value">値</param>
	void SetValue(const std::string& groupName, const std::string& key, Vector3 value);

	Vector3 GetValue(const std::string& groupName,const std::string& key);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();
private://メンバ関数
	//コンストラクタを封印
	GlobalVariables() = default;
	//デストラクタを封印
	~GlobalVariables() = default;
	//コピーコンストラクタ禁止
	GlobalVariables(const GlobalVariables&) = delete;
	//代入演算子を禁止
	GlobalVariables operator=(const GlobalVariables&) = delete;
public://静的メンバ変数
	//カメラのインスタンス
	static inline GlobalVariables* instance = nullptr;
	//終了処理をしたかのフラグ
	static inline bool isFinalize = false;
private://メンバ変数
	//全データ
	std::map<std::string, Group>dataList_;
};

