#include "ColliderManager.h"
#include "func/Collision.h"
#include "Object3d.h"

void ColliderManager::Register(Collider* collider) {
	colliders_.push_back(collider);
}

void ColliderManager::Step() {
	for (Collider* collider : colliders_) {
		if (!collider) {
			continue;
		} else if (!collider->enabled) {
			continue;
		} else if (!collider->owner) {
			continue;
		}

		//OBBの値の更新
		collider->obb.size = collider->owner->transformData.scale;
		collider->obb.rotate = collider->owner->transformData.rotate;
		collider->obb.center = collider->owner->object3d->GetWorldPos();
	}

	for (uint32_t i = 0; i < colliders_.size(); i++) {
		Collider* a = colliders_[i];
		if (!a) {
			continue;
		} else if (!a->enabled) {
			continue;
		} else if (!a->owner) {
			continue;
		}

		for (uint32_t j = i + 1; j < colliders_.size(); j++) {
			Collider* b = colliders_[j];
			if (!b) {
				continue;
			} else if (!b->enabled) {
				continue;
			} else if (!b->owner) {
				continue;
			}

			if (!Collision::IsCollision(a->obb, b->obb)) {
				continue;
			}

			if (a->onCollision) {
				a->onCollision(b->owner);
			}
			if (b->onCollision) {
				b->onCollision(a->owner);
			}
		}
	}
}
