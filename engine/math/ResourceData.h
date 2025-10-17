#pragma once
#include "Vector2.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include <cstdint>
#include <string>
#include <vector>

//頂点データ
struct VertexData {
	Vector4 position;//直行座標
	Vector2 texcoord;//UV座標
	Vector3 normal;//法線
};

//マテリアル
struct Material {
	Vector4 color;//色
	int32_t enableLighting;//ライティングするかどうかのフラグ
	float padding[3];
	Matrix4x4 uvMatrix;//UVTransform
	float shininess;//光沢度
};

//平行光源
struct DirectionalLight {
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度
	int32_t isLambert;//lambertにするかどうか
	int32_t isBlinnPhong;//BlinnPhongReflectionを行うかどうか
};

//点光源
struct PointLight {
	Vector4 color;//ライトの色
	Vector3 position;//ライトの位置
	float intensity;//輝度
};

//マテリアルデータ
struct MaterialData {
	std::string textureFilePath;
	uint32_t srvIndex;
};


//モデルデータの構造体
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

//カメラのデータの構造体
struct CameraForGPU {
	Vector3 worldPosition;
};