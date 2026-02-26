#pragma once
#include "ActorData.h"
#include "engine/math/Matrix4x4.h"
#include "PrimitiveData.h"
#include <vector>
#include <string>

//前方宣言
class Object3dCommon;
class Camera;

/// <summary>
/// 弾
/// </summary>
class Bullet {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Bullet() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Bullet();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">3Dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(Object3dCommon* object3dCommon, Camera* camera);

	/// <summary>
	/// 弾を発射する
	/// </summary>
	/// <param name="isShooting">弾を発射するかどうか</param>
	void Fire(bool isShooting);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 衝突したとき
	/// </summary>
	/// <param name="index">弾のデータインデックス</param>
	void OnCollision(uint32_t index);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 発射地点のセッター
	/// </summary>
	/// <param name="shootingPosition">発射地点</param>
	void SetShootingPosition(const Vector3& shootingPosition);

	/// <summary>
	/// 弾を保持しているオブジェクトのワールド行列のセッター
	/// </summary>
	/// <param name="sourceWorldMatrix">弾を保持しているオブジェクトのワールド行列</param>
	void SetSourceWorldMatrix(const Matrix4x4& sourceWorldMatrix);

	/// <summary>
	/// スピードのセッター
	/// </summary>
	/// <param name="speed">スピード</param>
	void SetSpeed(float speed);

	/// <summary>
	/// サイズのセッター
	/// </summary>
	/// <param name="size">サイズ</param>
	void SetSize(const Vector3& size);

	/// <summary>
	/// 角度のセッター
	/// </summary>
	/// <param name="angle">角度</param>
	void SetAngle(const Vector3& angle);

	/// <summary>
	/// 生存範囲のセッター
	/// </summary>
	/// <param name="aliveRange">生存範囲</param>
	void SetAliveRange(float aliveRange);

	/// <summary>
	/// 弾を発射できる最大値のセッター
	/// </summary>
	/// <param name="maxBulletCount">弾の発射できる最大値</param>
	void SetMaxBulletCount(uint32_t maxBulletCount);

	/// <summary>
	/// モデルのセッター
	/// </summary>
	/// <param name="modelName">モデル名</param>
	void SetModel(const std::string& modelName);

	/// <summary>
	/// 弾のデータリストのゲッター
	/// </summary>
	/// <returns>弾のデータ</returns>
	std::vector<BulletData>& GetBulletData();
private://メンバ関数
	/// <summary>
	/// 弾の生成
	/// </summary>
	/// <param name="bulletData">弾のデータ</param>
	void CreateBullet(BulletData& bulletData);
private://定数
	//弾の数
	static inline const int32_t kMaxBulletCount = 20;
private://メンバ変数
	//3Dオブジェクトの共通部分
	Object3dCommon* object3dCommon_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;
	// 弾のデータ
	std::vector<BulletData>bulletDatas_;
	//弾の発射地点
	Vector3 shootingPosition_ = {};
	//マテリアル
	Material material_ = {};
	//弾を保持しているオブジェクトのワールド行列
	Matrix4x4 sourceWorldMatrix_ = Matrix4x4::Identity4x4();
	//スピード
	float speed_ = 1.0f;
	//サイズ
	Vector3 size_ = { 1.0f,1.0f,1.0f };
	//角度
	Vector3 angle_ = {};
	//生存範囲
	float aliveRange_ = 0.0f;
	//弾が発射できる最大値
	uint32_t maxBulletCount_ = 1;
	//ヒットボックスのサイズ
	Vector3 hitBoxScale_ = { 1.0f,1.0f,1.0f };
	//モデル名
	std::string modelName_ = "bullet";
};

