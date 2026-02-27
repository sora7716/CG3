#pragma once
#include "RenderingData.h"
#include "ResourceData.h"
#include "PrimitiveData.h"
#include <functional>

class Object3d;
class WireframeObject3d;

//タグ
enum class Tag {
	kPlayer,
	kEnemy,
	kPlayerBullet,
	kEnemyBullet,
	kWall
};

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
	Tag tag;
};

//弾
struct BulletData {
	GameObject gameObject;
	Vector3 direction;
	Vector3 shootingPoint;
	float aliveRange;
};

//Collider
//owner: どの物体の当たり判定
//obb: 当たり判定の形
//isTrigger: 押し戻ししない
//isEnabled: 無効化用
//onCollision: 衝突したときに呼ばれる
struct Collider {
	GameObject* owner;
	OBB obb;
	bool isTrigger;
	bool isEnabled;
	std::function<void(GameObject* other)>onCollision;
};