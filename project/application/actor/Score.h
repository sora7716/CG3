#pragma once
#include "engine/2d/TextData.h"
#include "engine/math/RenderingData.h"
#include <cstdint>
#include <array>
#include <Vector2.h>
#include <Vector2.h>

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
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 位置のセッター
	/// </summary>
	/// <param name="position">位置</param>
	void SetPosition(const Vector2& position);

	/// <summary>
	/// スケールのセッター
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetScale(const Vector2& scale);

	/// <summary>
	/// 色のセッター
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Vector4& color);

	/// <summary>
	/// テキストのサイズのセッター
	/// </summary>
	/// <param name="size">テキストサイズ</param>
	void SetTextSize(float size);

	/// <summary>
	/// スコアのセッター
	/// </summary>
	/// <param name="score">スコア</param>
	void SetScore(int32_t score);

	/// <summary>
	/// スコアのゲッター
	/// </summary>
	/// <returns>スコア</returns>
	int32_t GetScore()const;
public://定数
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

