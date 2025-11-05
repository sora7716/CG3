#pragma once
#include "engine/math/Vector3.h"
#include "externals/nlohmann/json.hpp"
#include <variant>
#include <cstdint>
#include <string>
#include <map>
#include <cassert>

//nlohmann::jsonをjsonという名前で扱う
using json = nlohmann::json;

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
	/// 値のセッター
	/// </summary>
	/// <typeparam name="T">テンプレート</typeparam>
	/// <param name="groupName">グループ名</param>
	/// <param name="key">検索キー</param>
	/// <param name="value">値</param>
	template <typename T>
	void SetValue(const std::string& groupName, const std::string& key, T value);

	/// <summary>
	/// 項目の追加
	/// </summary>
	/// <typeparam name="T">テンプレート</typeparam>
	/// <param name="groupName">グループ名</param>
	/// <param name="key">検索キー</param>
	/// <param name="value">値</param>
	template <typename T>
	void AddItem(const std::string& groupName, const std::string& key, T value);

	/// <summary>
	/// 値のゲッター
	/// </summary>
	/// <typeparam name="T">テンプレート</typeparam>
	/// <param name="groupName">グループ名</param>
	/// <param name="key">検索キー</param>
	/// <returns>値</returns>
	template <typename T>
	T GetValue(const std::string& groupName, const std::string& key);

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

	/// <summary>
	/// 設定ファイルの書き出し
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void SaveFile(const std::string& groupName);
public:
	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// 設定ファイルの読み込み
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void LoadFile(const std::string& groupName);
public://静的メンバ変数
	//カメラのインスタンス
	static inline GlobalVariables* instance = nullptr;
	//終了処理をしたかのフラグ
	static inline bool isFinalize = false;
private://定数
	//グローバル変数の保存先ファイルパス
	static inline const std::string kDirectoryPath = "engine/resources/json/ini/";
private://メンバ変数
	//全データ
	std::map<std::string, Group>dataList_;
};

//値のセッター
template<typename T>
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, T value) {
	//グループの参照を取得
	Group& group = dataList_[groupName];

	//新しい項目データの設定
	Item newItem = {};
	newItem.value = value;

	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

//項目の追加
template<typename T>
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, T value) {
	if (dataList_[groupName].items.contains(key)) {
		return;
	}
	SetValue(groupName, key, value);
}

//値のゲッター
template<typename T>
inline T GlobalVariables::GetValue(const std::string& groupName, const std::string& key) {
	//グループが存在するか
	assert(dataList_.contains(groupName));

	//グループの参照を取得
	const Group& group = dataList_.at(groupName);

	//項目が存在するか
	assert(group.items.contains(key));

	//戻り値として検索キーの項目の値を返す
	return std::get<T>(group.items.at(key).value);
}
