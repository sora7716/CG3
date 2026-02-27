#pragma once
#include "ActorData.h"
#include <vector>
#include <func/Collision.h>

/// <summary>
/// 衝突の管理
/// </summary>
class ColliderManager {
public://メンバ関数
	/// <summary>
	/// コライダーを追加
	/// </summary>
	/// <param name="collider">追加するコライダー</param>
	void AddCollider(Collider* collider);

	/// <summary>
	/// コライダーを削除
	/// </summary>
	/// <param name="collider">削除するコライダー</param>
	void RemoveCollider(Collider* collider);

	/// <summary>
	/// 衝突判定を行う
	/// </summary>
	void ProcessCollision();
private://メンバ関数
	/// <summary>
	/// 追加していいか
	/// </summary>
	/// <param name="collider">コライダー</param>
	/// <returns>追加していいか</returns>
	bool IsRegistered(Collider* collider);

	/// <summary>
	/// コライダーをOBBに同期
	/// </summary>
	void SyncCollider();

	/// <summary>
	/// 衝突判定をチェック
	/// </summary>
	void CheckCollision();

	/// <summary>
    /// 衝突判定を行えるか
    /// </summary>
	/// <param name="collider">コライダー</param>
    /// <returns>衝突判定を行えるか</returns>
	bool IsActive(Collider* collider);

	/// <summary>
	/// 押し出し
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <param name="hitInfo">衝突情報</param>
	void Resolve(GameObject& self, const GameObject& other, HitInfo hit);
private://メンバ変数
	std::vector<Collider*>colliders_;
};

