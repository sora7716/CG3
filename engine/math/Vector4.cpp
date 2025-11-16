#include "Vector4.h"

//RGBのゲッター
const RGB Vector4::GetRGB()const {
	return { x,y,z };
}

//RGBのセッター
void Vector4::SetRGB(const RGB& rgb) {
	x = rgb.r;
	y = rgb.g;
	z = rgb.b;
}

//白のゲッター
Vector4 Vector4::MakeWhiteColor() {
	return Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}
