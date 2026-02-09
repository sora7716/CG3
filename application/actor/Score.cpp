#include "Score.h"
#include <sstream>
#include <iomanip>
#include "engine/2d/Text.h"

//デストラクタ
Score::~Score() {
	delete text_;
}

//スコアを加算
void Score::AddScore(int32_t addScore) {
	score += addScore;
}

//初期化
void Score::Initialize(Object2dCommon* object2dCommon) {
	//文字スタイルを作成
	std::ostringstream scoreText;
	scoreText << "SCORE : " << std::setw(kDigitCount) << std::setfill('0') << score;
	textStyle_.text = scoreText.str();
	textStyle_.font = "\0";
	textStyle_.size = 16.0f;
	textStyle_.color = Vector4::MakeWhiteColor();
	//文字のトランスフォームデータを作成
	transformData_.scale = { 200.0f,200.0f };
	transformData_.rotate = 0.0f;
	transformData_.translate = { 0.0f,0.0f };
	//文字の生成と初期化
	text_ = new Text();
	text_->Initialize(object2dCommon, "scoreText");
	text_->SetTextStyle(textStyle_);
}

//更新
void Score::Update() {
	std::ostringstream scoreText;
	scoreText << "SCORE : " << std::setw(kDigitCount) << std::setfill('0') << score;
	text_->SetText(scoreText.str());
	text_->Update();
}

//描画
void Score::Draw() {
	text_->Draw();
}
