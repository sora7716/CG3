#pragma once
#include <list>
#include "engine/math/RenderingData.h"
#include "engine/math/Vector4.h"
#include <random>

//前方宣言
class Camera;
class Model;

//AABB
struct AABB {
	Vector3 min;//最小値
	Vector3 max;//最大値
};

//パーティクルの情報をGPUに送るための構造体
struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

//パーティクル単体のデータ
struct Particle {
	TransformData transform;//SRVの情報
	Vector3 velocity;//速度
	Vector3 direction;//方向
	Vector4 color;//色
	float lifeTime;//生存時間
	float currentTime;//発生してからの
};

//発生源
struct Emitter {
	TransformData transform;//エミッターのTransform
	uint32_t count;//発生数
	float frequency;//発生頻度
	float frequencyTime;//頻度用時刻
};

//フィールドの加速度
struct AccelerationField {
	Vector3 acceleration;//加速度
	AABB area;//範囲
};

/// <summary>
/// パーティクルの発生源
/// </summary>
class ParticleEmitter {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleEmitter() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleEmitter() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Initialize(Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="instancingData">インスタンシングデータ</param>
	void Update(ParticleForGPU* instancingData);

	Model* GetModel()const;

	const uint32_t GetNumInstance()const;

	void SetCamera(Camera* camera);

private://メンバ関数
	/// <summary>
	/// パーティクルの生成
	/// </summary>
	/// <returns>新しいパーティクル</returns>
	Particle MakeNewParticle();

	/// <summary>
	/// ワールドトランスフォームの更新
	/// </summary>
	/// <param name="numInstance">インスタンス数</param>
	/// <param name="iterator"イテレータ></param>
	/// <param name="instancingData">インスタンシングデータ</param>
	void UpdateWorldTransform(uint32_t numInstance, auto iterator, ParticleForGPU* instancingData);

	/// <summary>
	/// パーティクルの発生
	/// </summary>
	/// <returns>パーティクル</returns>
	std::list<Particle>Emit();

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="aabb">AABB</param>
	/// <param name="point">point</param>
	/// <returns>衝突判定</returns>
	bool IsCollision(const AABB& aabb, const Vector3& point);

	/// <summary>
	/// 矩形状に移動させる
	/// </summary>
	/// <param name="translate">平行移動</param>
	/// <param name="direction">移動する向き</param>
	/// <param name="velocity">速度</param>
	void EmitOnRect(Vector3& translate, const Vector3& direction, const Vector3& velocity);
public://静的メンバ変数
	//パーティクルの数
	static const uint32_t kNumMaxInstance = 1024;
private://メンバ変数
	//カメラ
	Camera* camera_ = nullptr;
	//モデル
	Model* model_ = nullptr;
	//ランダムエンジン
	std::mt19937 randomEngine_;
	//パーティクルのデータ
	std::list<Particle> particles_ = {};
	//ワールドマトリックス
	Matrix4x4 worldMatrix_ = {};
	//生存しているパーティクルの数
	uint32_t numInstance_ = 0;
	//発生源
	Emitter emitter_ = {
		.transform = {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}},
		.count = 1,
		.frequency = 0.1f,//発生頻度
		.frequencyTime = 0.0f//発生頻度用の時刻,0.0fで初期化
	};
	//フィールドの加速度
	AccelerationField accelerationField_ = {
		.acceleration = {15.0f,0.0f,0.0f},
		.area = {{-1.0f,-1.0f,-1.0f},1.0f,1.0f,1.0f}
	};
};

