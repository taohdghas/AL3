#pragma once
#include "Vector3.h"
// 左右
enum class LRDirection {
	kRight,
	kLeft,
};
// 角
enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上

	kNumCorner // 要素数
};
// マップとの当たり判定情報
struct CollisionMapInfo {
	// 天井衝突フラグ
	bool ceilCollision = false;
	// 着地フラグ
	bool onGround = false;
	// 壁接触フラグ
	bool wallhit = false;
	// 移動量
	Vector3 moveAmount;
};
