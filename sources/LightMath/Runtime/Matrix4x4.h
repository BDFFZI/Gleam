#pragma once
#include "Matrix3x3.h"
#include "Vector4.h"

struct Matrix4x4
{
    static const Matrix4x4 zero;
    static const Matrix4x4 identity;

    static Matrix4x4 Translate(Vector3 position);
    static Matrix4x4 Rotate(EulerAngles rotation);
    static Matrix4x4 Scale(Vector3 scale);
    static Matrix4x4 TRS(Vector3 position, EulerAngles rotation, Vector3 scale);
    /**
     * 投射到的剪辑空间遵从从右到左、从下到上都为[-1,1]，深度从近到远为[0-1]
     * @param right 
     * @param top 
     * @param nearClipPlane 
     * @param farClipPlane 
     * @return 
     */
    static Matrix4x4 Ortho(float right, float top, float nearClipPlane, float farClipPlane);
    /**
     * 
     * @param fieldOfView 视野上下角度
     * @param aspectRatio 视野宽高比（宽度/高度）
     * @param nearClipPlane 
     * @param farClipPlane 
     * @return 
     */
    static Matrix4x4 Perspective(float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane);

    //为了与图像接口兼容故采用按列存储
    float m00, m10, m20, m30;
    float m01, m11, m21, m31;
    float m02, m12, m22, m32;
    float m03, m13, m23, m33;

    Matrix4x4() = default;
    Matrix4x4(float m00, float m01, float m02, float m03,
              float m10, float m11, float m12, float m13,
              float m20, float m21, float m22, float m23,
              float m30, float m31, float m32, float m33);
    Matrix4x4(const Matrix3x3& matrix);

    float GetElement(int row, int column) const;
    Vector4 GetRow(int row);
    Vector4 GetColumn(int column);
    /**
     * 
     * @param row 
     * @param column 
     * @param sign 输出作为代数余子式时的符号
     * @return 
     */
    Matrix3x3 GetCofactor(int row, int column, int* sign = nullptr) const;
    float GetCofactorDeterminant(int row, int column) const;
    Matrix4x4 GetCofactorMatrix() const;
    Matrix4x4 GetTranspose();
    Matrix4x4 GetAdjoint() const;
    float GetDeterminant() const;
    Matrix4x4 GetInverse() const;

    void SetElement(int row, int column, float value);
    void SetRow(int row, Vector4 vector);
    void SetColumn(int column, Vector4 vector);
};

std::string to_string(const Matrix4x4& matrix);
Matrix4x4 operator*(const Matrix4x4& left, const Matrix4x4& right);
Vector4 operator*(const Matrix4x4& left, Vector4 right);
Matrix4x4 operator*(const Matrix4x4& left, float right);
Matrix4x4 operator/(const Matrix4x4& left, float right);
bool operator==(const Matrix4x4& left, const Matrix4x4& right);
