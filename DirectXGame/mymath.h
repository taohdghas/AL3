#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//Vector3の足し算
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator+=(const Vector3& v1, const Vector3& v2);
//Vector3の引き算
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator-=(const Vector3& v1, const Vector3& v2);
//代入演算子オーバーロード
//Vector3の掛け算
Vector3& operator*=(Vector3& v, float s);

//2項演算子オーバーロード
//Vector3の掛け算
const Vector3 operator*(const Vector3& v, float s);

//線形補間
double easeInOutSine(double x);
// 加算
Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

// スカラー倍
Vector3 Multiply(float scalar, const Vector3& v);

// 内積
float Dot(const Vector3& v1, const Vector3& v2);

// 長さの2乗
float LengthSquared(const Vector3& v);

// 長さ
float Length(const Vector3& v);

// 正規化
Vector3 Normalize(const Vector3& v);
// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
// x軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);
// y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);
// z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);