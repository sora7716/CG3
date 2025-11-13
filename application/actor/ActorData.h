#pragma once
#include "engine/math/RenderingData.h"

//ゲームオブジェクト
struct GameObject {
	TransformData transformData;
	Vector3 velocity;
	Vector3 direction;
};
