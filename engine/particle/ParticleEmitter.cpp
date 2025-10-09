#define NOMINMAX
#include "ParticleEmitter.h"
#include "engine/math/func/Math.h"
#include "engine/camera/Camera.h"
#include "engine/particle/ParticleCommon.h"
#include "engine/3d/Model.h"

//初期化
void ParticleEmitter::Initialize(Model* model) {
	//カメラを設定
	camera_ = ParticleCommon::GetInstance()->GetDefaultCamera();
	//モデルを設定
	model_ = model;
	//乱数エンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator());
}

//更新
void ParticleEmitter::Update(ParticleForGPU* instancingData) {
	//生存しているパーティクルの数を0に初期化
	numInstance_ = 0;

	//更新処理
	for (auto it = particles_.begin(); it != particles_.end();) {
		if (numInstance_ < kNumMaxInstance) {
			if ((*it).lifeTime <= (*it).currentTime) {
				it = particles_.erase(it); //生存期間を過ぎたらパーティクルをlistから削除
				continue;//削除したので次のループへ
			}
			// パーティクルの色を設定
			instancingData[numInstance_].color.SetRGB((*it).color.GetRGB());
			//Field内のParticleには加速度を適用する
			if (IsCollision(accelerationField_.area, (*it).transform.translate)) {
				//(*it).velocity += accelerationField_.acceleration * Math::kDeltaTime;
			}
			//移動
			EmitOnRect((*it).transform.translate, (*it).direction, (*it).velocity);
			//(*it).transform.translate += (*it).velocity * Math::kDeltaTime;
			//経過時間を足す
			(*it).currentTime += Math::kDeltaTime;
			float alpha = 1.0f - ((*it).currentTime / (*it).lifeTime);
			instancingData[numInstance_].color.w = alpha;
			//ワールドトランスフォームの更新
			UpdateWorldTransform(numInstance_, it, instancingData);
			//生きているパーティクルの数を記録
			numInstance_++;

		}
		//次のイテレータに進める
		it++;
	}

	//Emitterの更新
	emitter_.frequencyTime += Math::kDeltaTime;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		particles_.splice(particles_.end(), Emit());
		emitter_.frequencyTime -= emitter_.frequency;//余計に過ぎた時間も加味して頻度を計算する
	}
}

Model* ParticleEmitter::GetModel() const {
	return model_;
}

const uint32_t ParticleEmitter::GetNumInstance() const {
	return numInstance_;
}

void ParticleEmitter::SetCamera(Camera* camera) {
	camera_ = camera;
}

//パーティクルの生成
Particle ParticleEmitter::MakeNewParticle() {
	//位置と速度を[-1.0f,1.0f]でランダムに設定
	std::uniform_real_distribution<float>distribution(-1.0f, 1.0f);
	//パーティクルの初期化
	Particle particle;
	//拡縮
	particle.transform.scale = { 1.0f, 1.0f, 1.0f };
	//回転
	particle.transform.rotate = { 0.0f, Math::kPi, 0.0f };
	//位置
	Vector3 randomTranslate = { distribution(randomEngine_), distribution(randomEngine_), distribution(randomEngine_) };
	//パーティクルの位置を発生源を中心に設定
	particle.transform.translate = emitter_.transform.translate + randomTranslate;
	//速度
	particle.velocity = { 1.0f,1.0f,1.0f };
	//移動する向き
	particle.direction = { distribution(randomEngine_), distribution(randomEngine_), distribution(randomEngine_) };
	//向きを正規化
	particle.direction = particle.direction.Normalize();
	//色の値を[0.0f,1.0f]でランダムに設定
	std::uniform_real_distribution<float>distColor(0.0f, 1.0f);
	//色
	particle.color = { distColor(randomEngine_), distColor(randomEngine_), distColor(randomEngine_),1.0f };

	//生存時間
	std::uniform_real_distribution<float>distTime(1.0f, 3.0f);
	particle.lifeTime = distTime(randomEngine_);
	particle.currentTime = 0.0f;

	return particle;
}

//ワールドトランスフォームの更新
void ParticleEmitter::UpdateWorldTransform(uint32_t numInstance, auto iterator, ParticleForGPU* instancingData) {
	//wvpの書き込み
	if (camera_) {
		if (model_) {
			//モデルがあったらAffine行列を入れる
			worldMatrix_ = Rendering::MakeAffineMatrix((*iterator).transform);
		} else {
			//モデルがないならビルボード行列を入れる
			worldMatrix_ = Rendering::MakeBillboardAffineMatrix(camera_->GetWorldMatrix(), (*iterator).transform);
		}
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();

		instancingData[numInstance].WVP = worldMatrix_ * viewProjectionMatrix;
	} else {
		instancingData[numInstance].WVP = worldMatrix_;
	}
	//ワールド行列を送信
	instancingData[numInstance].World = worldMatrix_;
}

//パーティクルの発生
std::list<Particle> ParticleEmitter::Emit() {
	std::list<Particle>particles;
	for (uint32_t i = 0; i < emitter_.count; i++) {
		particles.push_back(MakeNewParticle());
	}
	return particles;
}

//衝突判定
bool ParticleEmitter::IsCollision(const AABB& aabb, const Vector3& point) {
	AABB temp = aabb;
	Vector3 tNear;
	Vector3 tFar;

	temp.min.x = (aabb.min.x - point.x);
	temp.max.x = (aabb.max.x - point.x);

	temp.min.y = (aabb.min.y - point.y);
	temp.max.y = (aabb.max.y - point.y);

	temp.min.z = (aabb.min.z - point.z);
	temp.max.z = (aabb.max.z - point.z);

	tNear.x = std::min(temp.min.x, temp.max.x);
	tFar.x = std::max(temp.min.x, temp.max.x);

	tNear.y = std::min(temp.min.y, temp.max.y);
	tFar.y = std::max(temp.min.y, temp.max.y);

	tNear.z = std::min(temp.min.z, temp.max.z);
	tFar.z = std::max(temp.min.z, temp.max.z);

	float tMin = std::max(tNear.x, std::max(tNear.z, tNear.y));
	float tMax = std::min(tFar.x, std::min(tFar.z, tFar.y));
	bool isCollision = false;
	if (tMin <= tMax) {
		if (tMin * tMax < 0.0f) {
			isCollision = true;
		}
		if (0.0f <= tMin && tMin <= 1.0f || 0.0f <= tMax && tMax <= 1.0f) {
			isCollision = true;
		}
	} else {
		isCollision = false;
	}
	return isCollision;
}

//矩形上にパーティクルを発生させる
void ParticleEmitter::EmitOnRect(Vector3& translate, const Vector3& direction, const Vector3& velocity) {
	translate += velocity * direction * Math::kDeltaTime;
}
