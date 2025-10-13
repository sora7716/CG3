#pragma once
#include "engine/math/RenderingData.h"

//ゲームオブジェクト
struct GameObect {
	TransformData transfromData;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 direction;
};
