#pragma once
#include "engine/2d/TextData.h"
#include "engine/math/RenderingData.h"
#include <cstdint>
#include <array>

//前方宣言
class Object2dCommon;
class Text;

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
	~Score();

	/// <summary>
	/// スコアの加算
	/// </summary>
	/// <param name="addScore">スコアを加算</param>
	static void AddScore(int32_t addScore);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object2dCommon">2Dオブジェクトの共通部分</param>
	void Initialize(Object2dCommon* object2dCommon);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
private://定数
	//桁数
	static inline const int32_t kDigitCount = 6;
private://静的メンバ変数
	static inline int32_t score = 0;
private://メンバ変数
	//テキスト
	Text* text_ = nullptr;
	//テキストスタイル
	TextStyle textStyle_ = {};
	//トランスフォームデータ
	Transform2dData transformData_ = { {100.0f,100.0f},{},{} };
};

