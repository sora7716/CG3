#pragma once
#include <cstdint>
#include <array>

/// <summary>
/// スコア
/// </summary>
class Score {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Score() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Score() = default;

	/// <summary>
	/// スコアの加算
	/// </summary>
	/// <param name="addScore">スコアを加算</param>
	static void AddScore(int32_t addScore);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
public://静的メンバ変数
	static inline int32_t score = 0;
private://定数
	//桁数
	static inline const int32_t kDigitCount = 6;
private://メンバ変数
	//各桁の情報を保持する
	std::array<int32_t, kDigitCount> numbers_ = {};
};

