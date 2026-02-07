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
	//ワイヤーフレームオブジェクトの共通部分
	WireframeObject3dCommon::GetInstance()->Initialize(core_->GetDirectXBase(), core_->GetSRVManager(), core_->GetModelManager());
	//パーティクルの共通部分の初期化
	ParticleCommon::GetInstance()->Initialize(core_->GetDirectXBase(), core_->GetSRVManager(), core_->GetTextureManager());
	//ゲームオブジェクトのリスト
	GameObjectList::GetInstance()->Initialize(core_.get());
	//カメラの設定
	core_->GetObject2dCommon()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	core_->GetObject3dCommon()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	WireframeObject3dCommon::GetInstance()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	ParticleCommon::GetInstance()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	//シーンの管理
	SceneManager::GetInstance()->Initialize(core_.get());
}

//更新
void Framework::Update() {
	//入力処理
	core_->GetInput()->Update();
	//カメラの管理
	core_->GetCameraManager()->Update();
	//シーンの管理
	SceneManager::GetInstance()->Update();
}

//終了
void Framework::Finalize() {
	//オーディオの管理
	AudioManager::GetInstance()->Finalize();
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
		if (core_->GetInput()->TriggerKey(DIK_ESCAPE) && core_->GetInput()->PressKey(DIK_LSHIFT)) {
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
