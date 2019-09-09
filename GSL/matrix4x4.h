#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "mathfwd.h"
#include "vector3d.h"
#include "vector4d.h"
#include "gltypes.h"
#include <iostream>
#include <iomanip>
#include <array>

namespace gsl
{

class Matrix4x4
{
private:
    // TODO: Change structure so that matrices doesn't need to be transposed to send to OpenGL.
    GLfloat data[16];

public:
    // Default constructor. Initializes the matrix with a diagonal at the identity axis.
    Matrix4x4(GLfloat identity);
    Matrix4x4(const gsl::Vector4D& diagonal = gsl::Vector4D{1.f, 1.f, 1.f, 1.f});
    Matrix4x4(std::initializer_list<GLfloat> values);
    explicit Matrix4x4(const std::array<gsl::Vector4D, 4>& vectors);

    Matrix4x4 identity() const;
    void setToIdentity();

    bool inverse();

    void translateX(GLfloat x = 0.f);
    void translateY(GLfloat y = 0.f);
    void translateZ(GLfloat z = 0.f);

    void setPosition(GLfloat x = 0.f, GLfloat y = 0.f, GLfloat z = 0.f);
    gsl::Vector3D getPosition();

    //Rotate using EulerMatrix
    void rotateX(GLfloat degrees = 0.f);
    void rotateY(GLfloat degrees = 0.f);
    void rotateZ(GLfloat degrees = 0.f);
//    void rotate(GLfloat angle, Vector3D vector);
//    void rotate(GLfloat angle, GLfloat xIn, GLfloat yIn, GLfloat zIn);

    void scale(Vector3D s);
    void scale(GLfloat uniformScale);
    void scale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);

    const GLfloat* constData() const;

    void transpose();
    Matrix4x4 transposed() const;

    Matrix4x4& setOrtho(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat nearPlane, GLfloat farPlane);
    Matrix4x4& setFrustum(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    Matrix4x4& setPersp(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane);

    static Matrix4x4 ortho(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat nearPlane, GLfloat farPlane);
    static Matrix4x4 frustum(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    static Matrix4x4 persp(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane);

    void setLookAt(const Vector3D &eye, const Vector3D &center, const Vector3D &up_axis);
    static Matrix4x4 lookAtRotation(const Vector3D &from, const Vector3D &to, const Vector3D &up_axis = gsl::Vector3D{0.f, 1.f, 0.f});

    /** Generates a View Matrix from the given vectors
     * Combines the lookAtRotation matrix with a translation matrix translated by the from vector
     * which combines to a viewMatrix to use by a camera.
     * @param from - world position point to translate from and also where rotation starts
     * @param to - world position point to rotate towards
     * @param up_axis - the world up-axis. Defaults to [0, 1, 0]
     */
    static Matrix4x4 viewMatrix(const Vector3D &from, const Vector3D &to, const Vector3D &up_axis = gsl::Vector3D{0.f, 1.f, 0.f});

    void setRotationToVector(const Vector3D &direction, Vector3D up = Vector3D(0.f,1.f,0.f));

    void translate(GLfloat x = 0.f, GLfloat y = 0.f, GLfloat z = 0.f);
    void translate(Vector3D positionIn);
    static Matrix4x4 translation(const Vector3D& trans);

    Matrix2x2 toMatrix2();
    Matrix3x3 toMatrix3() const;

    GLfloat& operator()(const int &y, const int &x);
    GLfloat operator()(const int &y, const int &x) const;

    Matrix4x4 operator*(const Matrix4x4 &other);

    Vector4D operator*(const Vector4D &other);

    friend std::ostream& operator<<(std::ostream &output, const Matrix4x4 &mIn)
    {
        output << std::setprecision(4) <<
                  "{" << mIn.data[0] << "\t, " << mIn.data[4] << "\t, " << mIn.data[8] << "\t, " << mIn.data[12] << "}\n" <<
                  "{" << mIn.data[1] << "\t, " << mIn.data[5] << "\t, " << mIn.data[9] << "\t, " << mIn.data[13] << "}\n" <<
                  "{" << mIn.data[2] << "\t, " << mIn.data[6] << "\t, " << mIn.data[10] << "\t, " << mIn.data[14] << "}\n" <<
                  "{" << mIn.data[3] << "\t, " << mIn.data[7] << "\t, " << mIn.data[11] << "\t, " << mIn.data[15] << "}\n";
        return output;
    }
    GLfloat getFloat(int space);
};

} //namespace

#endif // MATRIX4X4_H
