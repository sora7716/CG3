#include "Player.h"
#include "engine/3d/Object3d.h"
#include "engine/camera/Camera.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/input/Input.h"
#include "engine/math/func/Math.h"
#include "engine/debug/GlobalVariables.h"
#include "engine/worldTransform/WorldTransform.h"

//初期化
void Player::Initialize(Camera* camera, const std::string& modelName) {
	//入力
	input_ = Input::GetInstance();

	//カメラ
	camera_ = camera;

	//トランスフォームの初期化
	playerData_.gameObject.transformData = {
		.scale = Vector3::MakeAllOne(),
		.rotate = {},
		.translate = {}
	};
	playerData_.gameObject.velocity = { 5.0f,5.0f,0.0f };

	//3Dオブジェクトの生成と初期化
	object3d_ = new Object3d();
	object3d_->Initialize(camera_);
	object3d_->SetModel(modelName);

	//マテリアルの初期化
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.shininess = 10.0f;
	material_.uvMatrix = Matrix4x4::Identity4x4();

	//調整項目
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//調整項目のグループの生成
	GlobalVariables::GetInstance()->CreateGroup(groupName_);
	globalVariables->AddItem(groupName_, "color", material_.color);
	globalVariables->AddItem(groupName_, "enableLighting", material_.enableLighting);
	globalVariables->AddItem(groupName_, "shininess", material_.shininess);
	globalVariables->AddItem(groupName_, "velocity", playerData_.gameObject.velocity);
	globalVariables->AddItem(groupName_, "translate", playerData_.gameObject.transformData.translate);
}

//更新
void Player::Update() {
	//調整項目を適応
	ApplyGlobalVariables();

	//トランスフォームのセット
	object3d_->SetTransform(playerData_.gameObject.transformData);

	//マテリアルのセット
	object3d_->GetModel()->SetMaterial(material_);

	//スペースを押した瞬間
	if (input_->TriggerKey(DIK_SPACE)) {
		bulletList_.push_back(CreateBullet());
	}

	for (std::list<Bullet>::iterator itBullet = bulletList_.begin(); itBullet != bulletList_.end(); itBullet++) {
		if (itBullet->isAlive) {
			//飛ばす
			itBullet->gameObject.transformData.translate += itBullet->gameObject.velocity;

			//弾の情報を設定
			itBullet->object3d->SetTransform(itBullet->gameObject.transformData);
			itBullet->object3d->GetModel()->SetMaterial(material_);

			//弾の生存させるか
			Vector3 currentPos = itBullet->gameObject.transformData.translate;
			float length = (currentPos - itBullet->shootingPoint).Length();
			if (itBullet->aliveRange < length) {
				itBullet->isAlive = false;
				//itBullet = bulletList_.erase(itBullet);
				continue;
			}

			//弾の更新
			itBullet->object3d->Update();
		}
	}

	//3Dオブジェクトの更新
	object3d_->Update();
}

//描画
void Player::Draw() {
	//3Dオブジェクトの描画
	object3d_->Draw();

	//弾の描画
	for (const Bullet& bullet : bulletList_) {
		if (bullet.isAlive) {
			bullet.object3d->Draw();
		}
	}

	//移動
	Move();
}

//デバッグ用
void Player::Debug() {
#ifdef USE_IMGUI
	ImGui::DragFloat3("rotate", &playerData_.gameObject.transformData.rotate.x, 0.1f);
	ImGui::DragFloat3("position", &playerData_.gameObject.transformData.translate.x, 0.1f);
#endif // USE_IMGUI
}

//終了
void Player::Finalize() {
	//3Dオブジェクトの解放
	delete object3d_;
	object3d_ = nullptr;
	for (Bullet& bullet : bulletList_) {
		delete bullet.object3d;
		bullet.object3d = nullptr;
	}
	bulletList_.clear();
}

//カメラのセッター
void Player::SetCamera(Camera* camera) {
	object3d_->SetCamera(camera);
}

//位置のセッター
void Player::SetPosition(const Vector3& positBulletion) {
	playerData_.gameObject.transformData.translate = positBulletion;
}

//オブジェクト3dのゲッター
Object3d* Player::GetObject3d() {
	return object3d_;
}

//トランスフォームデータのゲッター
TransformData Player::GetTransformData() {
	return playerData_.gameObject.transformData;
}

//速度のゲッター
Vector3 Player::GetVelocity() {
	return playerData_.gameObject.velocity;
}

//移動
void Player::Move() {
	//移動しているかのフラグのリセット
	playerData_.isMove = false;

	//前後
	if (input_->PressKey(DIK_UP)) {
		playerData_.gameObject.direction.y = 1.0f;
		playerData_.isMove = true;
	} else if (input_->PressKey(DIK_DOWN)) {
		playerData_.gameObject.direction.y = -1.0f;
		playerData_.isMove = true;
	} else {
		playerData_.gameObject.direction.y = 0.0f;
	}

	//左右
	if (input_->PressKey(DIK_RIGHT)) {
		playerData_.gameObject.direction.x = 1.0f;
		playerData_.isMove = true;
	} else if (input_->PressKey(DIK_LEFT)) {
		playerData_.gameObject.direction.x = -1.0f;
		playerData_.isMove = true;
	} else {
		playerData_.gameObject.direction.x = 0.0f;
	}

	//移動
	playerData_.gameObject.transformData.translate += (playerData_.gameObject.velocity * playerData_.gameObject.direction.Normalize()) * Math::kDeltaTime;
}

//調整項目を適応
void Player::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//各調整項目を適応
	material_.color = globalVariables->GetValue<Vector4>(groupName_, "color");
	material_.enableLighting = globalVariables->GetValue<int32_t>(groupName_, "enableLighting");
	playerData_.gameObject.velocity = globalVariables->GetValue<Vector3>(groupName_, "velocity");
	if (playerData_.isMove) {
		globalVariables->SetValue(groupName_, "translate", playerData_.gameObject.transformData.translate);
	}
	playerData_.gameObject.transformData.translate = globalVariables->GetValue<Vector3>(groupName_, "translate");
}

//弾の生成
Bullet Player::CreateBullet() {
	Bullet bullet = {};
	bullet.gameObject.transformData.scale = Vector3::MakeAllOne();
	bullet.isAlive = true;
	//弾の位置を設定
	bullet.gameObject.transformData.translate = object3d_->GetWorldPos();
	//射撃地点を取得
	bullet.shootingPoint = bullet.gameObject.transformData.translate;

	//3Dモデルの生成
	bullet.object3d = new Object3d();
	bullet.object3d->Initialize(camera_);
	bullet.aliveRange = 10.0f;
	bullet.object3d->SetModel("bullet");

	//弾の速度を設定
	const float kBulletSpeed = 0.1f;
	bullet.gameObject.velocity = { 0.0f,0.0f,kBulletSpeed };
	bullet.gameObject.velocity = Math::TransformNormal(bullet.gameObject.velocity, object3d_->GetWorldTransform()->GetWorldMatrix());
	return bullet;
}
