#pragma once
#include "engine/math/RenderingData.h"
#include "engine/math/ResourceData.h"

class Object3d;

//ゲームオブジェクト
struct GameObject {
	TransformData transformData;
	Vector3 velocity;
	Vector3 direction;
	Material material;
};

//プレイヤーデータ
struct PlayerData {
	GameObject gameObject;
	Object3d* object3d;
	bool isMove;
};

//弾
struct BulletData {
	GameObject gameObject;
	Object3d* object3d;
	Vector3 shootingPoint;
	float aliveRange;
	bool isAlive;
};