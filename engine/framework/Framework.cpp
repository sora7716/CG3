#include "Framework.h"
#include "engine/2d/TextureManager.h"
#include "application/actor/GameObjectList.h"
#include "engine/math/func/Math.h"
#include "engine/audio/AudioManager.h"
#include "engine/input/Input.h"
#include "engine/3d/ModelManager.h"
#include "engine/particle/ParticleManager.h"

//初期化
void Framework::Initialize() {
	//エンジンの核
	core_ = std::make_unique<Core>();
	core_->Initialize();
	//SRVの管理
	SRVManager::GetInstance()->Initialize(core_->GetDirectXBase());
	//入力処理
	Input::GetInstance()->Initialize(core_->GetWinApi());
	//テクスチャ管理
	TextureManager::GetInstance()->Initialize(core_->GetDirectXBase());
	//モデルの管理
	ModelManager::GetInstance()->Initialize(core_->GetDirectXBase());
	//ImGuiの管理
	ImGuiManager::GetInstance()->Initialize(core_->GetDirectXBase(),core_->GetWinApi());
	//スプライトの共通部分
	SpriteCommon::GetInstance()->Initialize(core_->GetDirectXBase());
	//2Dオブジェクトの共通部分
	Object2dCommon::GetInstance()->Initialize(core_->GetDirectXBase());
	//3Dオブジェクトの共通部分
	Object3dCommon::GetInstance()->Initialize(core_->GetDirectXBase());
	//ワイヤーフレームオブジェクトの共通部分
	WireframeObject3dCommon::GetInstance()->Initialize(core_->GetDirectXBase());
	//パーティクルの共通部分の初期化
	ParticleCommon::GetInstance()->Initialize(core_->GetDirectXBase());
	//ゲームオブジェクトのリスト
	GameObjectList::GetInstance()->Initialize(core_.get());
	//カメラの設定
	Object2dCommon::GetInstance()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	Object3dCommon::GetInstance()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	WireframeObject3dCommon::GetInstance()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	ParticleCommon::GetInstance()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	//シーンの管理
	SceneManager::GetInstance()->Initialize(core_->GetDirectXBase(),core_->GetCameraManager());
}

//更新
void Framework::Update() {
	//入力処理
	Input::GetInstance()->Update();
	//カメラの管理
	core_->GetCameraManager()->Update();
	//シーンの管理
	SceneManager::GetInstance()->Update();
}

//終了
void Framework::Finalize() {
	//SRVの管理
	SRVManager::GetInstance()->Finalize();
	//入力
	Input::GetInstance()->Finalize();
	//テクスチャ管理
	TextureManager::GetInstance()->Finalize();
	//モデルの管理
	ModelManager::GetInstance()->Finalize();
	//ImGuiの管理
	ImGuiManager::GetInstance()->Finalize();
	//オーディオの管理
	AudioManager::GetInstance()->Finalize();
	//スプライトの共通部分
	SpriteCommon::GetInstance()->Finalize();
	//2Dオブジェクトの共通部分
	Object2dCommon::GetInstance()->Finalize();
	//3Dオブジェクトの共通部分
	Object3dCommon::GetInstance()->Finalize();
	//ワイヤーフレームの共通部分
	WireframeObject3dCommon::GetInstance()->Finalize();
	//パーティクルの共通部分
	ParticleCommon::GetInstance()->Finalize();
	//パーティクルマネージャー
	ParticleManager::GetInstance()->Finalize();
	//シーンの管理
	SceneManager::GetInstance()->Finalize();
	//シーンファクトリーの解放
	delete sceneFactory_;
	//ゲームオブジェクトリスト
	GameObjectList::GetInstance()->Finalize();
}

//ゲームループ
void Framework::Run() {
	//ゲームシステムの初期化
	Initialize();
	//ウィンドウの✖ボタンが押されるまでループ
	while (isEndRequest()) {
		//ゲームシステムの更新
		Update();
		//ゲームシステムの描画
		Draw();
#ifdef _DEBUG
		//エスケイプを押したらループを抜ける
		if (Input::GetInstance()->TriggerKey(DIK_ESCAPE) && Input::GetInstance()->PressKey(DIK_LSHIFT)) {
			break;
		}
#endif // _DEBUG
	}
	//ゲームシステムの終了
	Finalize();
}

//終了リクエスト
bool Framework::isEndRequest() {
	return core_->GetWinApi()->ProcesMessage();
}
