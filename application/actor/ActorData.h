#pragma once
#include "engine/math/RenderingData.h"
#include "engine/math/ResourceData.h"

class Object3d;
class WireframeObject3d;

//ゲームオブジェクト
struct GameObject {
	TransformData transformData;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 direction;
	Material material;
	Object3d* object3d;
	WireframeObject3d* hitBox;
	uint32_t isAlive;
};

//弾
struct BulletData {
	GameObject gameObject;
	Vector3 direction;
	Vector3 shootingPoint;
	float aliveRange;
};