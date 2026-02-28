#pragma once
#include "RenderingData.h"
#include "ResourceData.h"
#include "PrimitiveData.h"
#include <functional>
#include <memory>

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
	bool isAlive;
	bool isOnGround;
};

//描画時に必要な物
//object3d: オブジェクト3d
//hitBox: ヒットボックス
//material: マテリアル
struct RenderObject {
	std::unique_ptr<Object3d> object3d;
	std::unique_ptr<WireframeObject3d> hitBox;
	Material material;
};

//弾
struct BulletData {
	GameObject gameObject;
	RenderObject renderObject;
	Vector3 direction;
	Vector3 shootingPoint;
	float aliveRange;
};

//Colliderの状態　
struct ColliderState {
	Vector3* scalePtr;
	Vector3* rotatePtr;
	Vector3* translatePtr;
	Vector3* velocityPtr;
	Matrix4x4* worldMatrixPtr;
	bool* isOnGroundPtr;
	Tag tag;
};

//Collider
//owner: どの物体の当たり判定
//obb: 当たり判定の形
//isTrigger: 押し戻ししない
//isEnabled: 無効化用
//onCollision: 衝突したときに呼ばれる
struct Collider {
	ColliderState* owner;
	OBB obb;
	bool isTrigger;
	bool isEnabled;
	std::function<void(ColliderState* other)>onCollision;
};