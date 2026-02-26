#pragma once
#include "PrimitiveData.h"

/// <summary>
/// 衝突判定
/// </summary>
class Collision {
public://静的メンバ関数
	/// <summary>
	/// 球同士の衝突判定
	/// </summary>
	/// <param name="sphere1">球1</param>
	/// <param name="sphere2">球2</param>
	/// <returns>衝突したかどうか</returns>
	static bool IsCollision(const Sphere& sphere1, const Sphere& sphere2);

	/// <summary>
	/// AABB同士の衝突判定
	/// </summary>
	/// <param name="aabb1">aabb1</param>
	/// <param name="aabb2">aabb2</param>
	/// <returns>衝突したかのフラグ</returns>
	static bool IsCollision(const AABB& aabb1, const AABB& aabb2);

	/// <summary>
	/// AABBと球の衝突判定
	/// </summary>
	/// <param name="aabb">aabb</param>
	/// <param name="sphere">球</param>
	/// <returns>衝突したかのフラグ</returns>
	static bool IsCollision(const AABB& aabb, const Sphere& sphere);

	/// <summary>
	/// OBBと球の衝突判定
	/// </summary>
	/// <param name="obb">obb</param>
	/// <param name="sphere">球</param>
	/// <returns>衝突したかのフラグ</returns>
	static bool IsCollision(const OBB& obb, const Sphere& sphere);

	/// <summary>
	/// 分離軸を使用したOBB同士の衝突判定
	/// </summary>
	/// <param name="obb1">obb1</param>
	/// <param name="obb2">obb2</param>
	/// <returns>衝突したかのフラグ</returns>
	static bool IsCollision(const OBB& obb1, const OBB& obb2);
};