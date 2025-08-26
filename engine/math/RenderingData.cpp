#include "RenderingData.h"

TransformData TransformData::operator/(const TransformData& transformData) {
	TransformData result = {};

	if (transformData.scale.x != 0.0f && transformData.scale.y != 0.0f && transformData.scale.z != 0.0f) {
		result.scale = scale / transformData.scale;
	} else {
		result.scale = scale;
	}
	if (transformData.rotate.x != 0.0f && transformData.rotate.y != 0.0f && transformData.rotate.z != 0.0f) {
		result.rotate = rotate / transformData.rotate;
	} else {
		result.rotate = rotate;
	}
	if (transformData.translate.x != 0.0f && transformData.translate.y != 0.0f && transformData.translate.z != 0.0f) {
		result.translate = translate / transformData.translate;
	} else {
		result.translate = translate;
	}

	return result;
}
