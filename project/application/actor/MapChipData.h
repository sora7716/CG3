#pragma once
#include <map>
//マップチップのタイプ
enum class MapChipType {
	kBlock,
	kBlank
};

//セルのデータ
struct CellData {
	MapChipType mapChipType;
	Vector4 color;
};

//マップチップデータ
struct MapChipData {
	std::vector<std::vector<CellData>>data;
};

namespace {
	//マップチップテーブル
	std::map<std::string, MapChipType> mapChipTable = {
		{"0",MapChipType::kBlank},
		{"1",MapChipType::kBlock},
	};
}