#pragma once
#include "engine/math/Vector3.h"
#include "engine/math/Vector2.h"
#include "engine/math/Matrix4x4.h"

//Transform情報
struct TransformData {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

//Transform2D情報
struct Transform2dData {
	Vector2 scale;
	float rotate;
	Vector2 translate;
};

//TransformationMatrix
struct TransformationMatrix {
	Matrix4x4 wvp;
	Matrix4x4 world;
	Matrix4x4 worldInverseTranspose;
};