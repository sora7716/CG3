#include "SpotLightManager.h"
#include <cassert>

//インスタンスのゲッター
SpotLightManager* SpotLightManager::GetInstance() {
	assert(!isFinalize && "GetInstance() called after Finalize()");
	if (instance == nullptr) {
		instance = new SpotLightManager();
	}
	return instance;
}

//終了
void SpotLightManager::Finalize() {
	delete instance;
	instance = nullptr;
	isFinalize = true;
}

//スポットライトを追加
void SpotLightManager::AddSpotLight(const std::string& name,Camera*camera) {
	//読み込み済みならカメラを検索
	if (spotLightList_.contains(name)) {
		//読み込み済みなら早期return
		return;
	}

	SpotLight* spotLight = new SpotLight();
	spotLight->Initialize(camera);

	//スポットライトをmapコンテナに格納する
	spotLightList_.insert(std::make_pair(name, std::move(spotLight)));
}

//スポットライトを検索
SpotLight* SpotLightManager::FindSpotLight(const std::string& name) {
	//読み込み済みモデルを検索
	if (spotLightList_.contains(name)) {
		//読み込み済みモデルを戻り値としてreturn
		return spotLightList_.at(name).get();
	}
	//ファイル名一致なし
	return nullptr;
}