#pragma once
#include "ActorData.h"
#include <vector>

/// <summary>
/// 衝突の管理
/// </summary>
class ColliderManager{
public://メンバ関数
	void Register(Collider* collider);

	void Step();
private://メンバ変数
	std::vector<Collider*>colliders_;
};

