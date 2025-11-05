#pragma once
#include "engine/math/RenderingData.h"

//ゲームオブジェクト
struct GameObect {
	TransformData transformData;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 direction;
};
