#pragma once
#include <Siv3D.hpp>

namespace Utils
{
	// 2Dグリッド上で、ピボットを中心に時計回り90度回転させた座標を返す
	inline Point RotatePointRight(const Point& pos, const Point& pivot)
	{
		// x軸右・y軸下向きの時計回り90度回転：(x, y) -> (y, -x)
		int32 dx = pos.x - pivot.x;
		int32 dy = pos.y - pivot.y;
		return Point{ pivot.x + dy, pivot.y - dx };
	}

	// 3D空間で、ピボットを中心にy軸回転させた座標を計算する
	inline Vec3 CalcOrbitPosition(const Vec3& pivot, const Vec3& target, double currentAngle)
	{
		double c = std::cos(currentAngle);
		double s = std::sin(currentAngle);
		Vec3 offset = target - pivot;

		return pivot + Vec3{
			offset.x * c + offset.z * s,
			offset.y,
			-offset.x * s + offset.z * c
		};
	}
}
