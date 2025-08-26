#pragma once
#include "engine/math/Vector3.h"
#include "engine/math/Vector2.h"
#include "engine/math/Matrix4x4.h"

//Transform情報
typedef struct TransformData {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
}TransformData;

//Transform2D情報
typedef struct Transform2dData {
	Vector2 scale;
	float rotate;
	Vector2 translate;
}Transform2dData;

//TransformationMatrix
typedef struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
}TransformationMatrix;