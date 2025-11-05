#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include <cassert>

GlobalVariables* GlobalVariables::GetInstance() {
	assert(!isFinalize && "GetInstance() called after Finalize()");
	if (instance == nullptr) {
		instance = new GlobalVariables();
	}
	return instance;
}

//グループの生成
void GlobalVariables::CreateGroup(const std::string& groupName) {
	//読み込み済みならカメラを検索
	if (dataList_.contains(groupName)) {
		//読み込み済みなら早期return
		return;
	}

	//グループの生成
	Group group = {};

	//カメラをmapコンテナに格納する
	dataList_.insert(std::make_pair(groupName, std::move(group)));
}

//更新
void GlobalVariables::Update() {
	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	if (!ImGui::BeginMenuBar()) {
		return;
	}

	//各グループについて
	for (std::map<std::string, Group>::iterator itGroup = dataList_.begin(); itGroup != dataList_.end(); itGroup++) {
		//グループ名を取得
		const std::string& groupName = itGroup->first;
		//グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) {
			continue;
		}

		//各項目について
		for (std::map<std::string, Item>::iterator itItem = group.items.begin(); itItem != group.items.end(); itItem++) {

			//項目名を取得
			const std::string& itemName = itItem->first;
			//項目の参照を取得
			Item& item = itItem->second;

			//項目の型によって表示を変える
			if (std::holds_alternative<int32_t>(item.value)) {
				//int32_t型の値を保持してれば
				int32_t* ptr = std::get_if<int32_t>(&item.value);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			} else if (std::holds_alternative<float>(item.value)) {
				//float型の値を保持してれば
				float* ptr = std::get_if<float>(&item.value);
				ImGui::SliderFloat(itemName.c_str(), ptr, 0.0f, 100.0f);
			} else if (std::holds_alternative<Vector3>(item.value)) {
				//Vector3型の値を保持してれば
				Vector3* ptr = std::get_if<Vector3>(&item.value);
				ImGui::SliderFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), -10.0f, 10.0f);
			}
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
}

//値のセット(int32_t)
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, int32_t value) {
	//グループの参照を取得
	Group& group = dataList_[groupName];

	//新しい項目データの設定
	Item newItem = {};
	newItem.value = value;

	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

//値のセット(float)
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {
	//グループの参照を取得
	Group& group = dataList_[groupName];

	//新しい項目データの設定
	Item newItem = {};
	newItem.value = value;

	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

//値のセット(Vector3)
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, Vector3 value) {
	//グループの参照を取得
	Group& group = dataList_[groupName];

	//新しい項目データの設定
	Item newItem = {};
	newItem.value = value;

	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

Vector3 GlobalVariables::GetValue(const std::string& groupName, const std::string& key) {

	Item item = dataList_[groupName].items[key];
	if (!std::holds_alternative<Vector3>(item.value)) {
		return Vector3{};
	}
	return *(std::get_if<Vector3>(&item.value));
}

void GlobalVariables::Finalize() {
	delete instance;
	instance = nullptr;
	isFinalize = true;
}