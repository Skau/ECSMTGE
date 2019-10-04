#include "matrix4x4.h"
//#include "quaternion.h"
#include "math_constants.h"
#include "math.h"
#include "matrix2x2.h"
#include "matrix3x3.h"
#include "quaternion.h"

#include <cmath>
#include <cstring> // For memcpy on linux
#include <utility>

namespace gsl
{

Matrix4x4::Matrix4x4(GLfloat identity)
    : data{
          identity, 0.f, 0.f, 0.f,
          0.f, identity, 0.f, 0.f,
          0.f, 0.f, identity, 0.f,
          0.f, 0.f, 0.f, identity
        }
{

}

Matrix4x4::Matrix4x4(const Vector4D &diagonal)
    : data{
          diagonal.x, 0.f, 0.f, 0.f,
          0.f, diagonal.y, 0.f, 0.f,
          0.f, 0.f, diagonal.z, 0.f,
          0.f, 0.f, 0.f, diagonal.w
        }
{

}

Matrix4x4::Matrix4x4(std::initializer_list<GLfloat> values)
{
    //Initializing the matrix class the same way as a 2d array
    int i = 0;
    for(auto it{values.begin()}; it != values.end() && i < 16; ++it, ++i)
        data[i] = *it;
}

Matrix4x4::Matrix4x4(const std::array<Vector4D, 4> &vectors)
{
    // Not sure whether this gets transposed or not. It is supposed to be row-wise.
    for (unsigned int outer{0}; outer < 4; ++outer) {
        for (unsigned int inner{0}; inner < 4; ++inner) {
            data[outer * 4 + inner] = vectors.at(outer)[static_cast<int>(inner)];
        }
    }
}

Matrix4x4 Matrix4x4::identity() const
{
    return gsl::Matrix4x4{};
}

void Matrix4x4::setToIdentity()
{
    *this =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

bool Matrix4x4::inverse()
{
    GLfloat inv[16], det;
    GLfloat invOut[16];

    int i;

    inv[0] = data[5]  * data[10] * data[15] -
            data[5]  * data[11] * data[14] -
            data[9]  * data[6]  * data[15] +
            data[9]  * data[7]  * data[14] +
            data[13] * data[6]  * data[11] -
            data[13] * data[7]  * data[10];

    inv[4] = -data[4]  * data[10] * data[15] +
            data[4]  * data[11] * data[14] +
            data[8]  * data[6]  * data[15] -
            data[8]  * data[7]  * data[14] -
            data[12] * data[6]  * data[11] +
            data[12] * data[7]  * data[10];

    inv[8] = data[4]  * data[9] * data[15] -
            data[4]  * data[11] * data[13] -
            data[8]  * data[5] * data[15] +
            data[8]  * data[7] * data[13] +
            data[12] * data[5] * data[11] -
            data[12] * data[7] * data[9];

    inv[12] = -data[4]  * data[9] * data[14] +
            data[4]  * data[10] * data[13] +
            data[8]  * data[5] * data[14] -
            data[8]  * data[6] * data[13] -
            data[12] * data[5] * data[10] +
            data[12] * data[6] * data[9];

    inv[1] = -data[1]  * data[10] * data[15] +
            data[1]  * data[11] * data[14] +
            data[9]  * data[2] * data[15] -
            data[9]  * data[3] * data[14] -
            data[13] * data[2] * data[11] +
            data[13] * data[3] * data[10];

    inv[5] = data[0]  * data[10] * data[15] -
            data[0]  * data[11] * data[14] -
            data[8]  * data[2] * data[15] +
            data[8]  * data[3] * data[14] +
            data[12] * data[2] * data[11] -
            data[12] * data[3] * data[10];

    inv[9] = -data[0]  * data[9] * data[15] +
            data[0]  * data[11] * data[13] +
            data[8]  * data[1] * data[15] -
            data[8]  * data[3] * data[13] -
            data[12] * data[1] * data[11] +
            data[12] * data[3] * data[9];

    inv[13] = data[0]  * data[9] * data[14] -
            data[0]  * data[10] * data[13] -
            data[8]  * data[1] * data[14] +
            data[8]  * data[2] * data[13] +
            data[12] * data[1] * data[10] -
            data[12] * data[2] * data[9];

    inv[2] = data[1]  * data[6] * data[15] -
            data[1]  * data[7] * data[14] -
            data[5]  * data[2] * data[15] +
            data[5]  * data[3] * data[14] +
            data[13] * data[2] * data[7] -
            data[13] * data[3] * data[6];

    inv[6] = -data[0]  * data[6] * data[15] +
            data[0]  * data[7] * data[14] +
            data[4]  * data[2] * data[15] -
            data[4]  * data[3] * data[14] -
            data[12] * data[2] * data[7] +
            data[12] * data[3] * data[6];

    inv[10] = data[0]  * data[5] * data[15] -
            data[0]  * data[7] * data[13] -
            data[4]  * data[1] * data[15] +
            data[4]  * data[3] * data[13] +
            data[12] * data[1] * data[7] -
            data[12] * data[3] * data[5];

    inv[14] = -data[0]  * data[5] * data[14] +
            data[0]  * data[6] * data[13] +
            data[4]  * data[1] * data[14] -
            data[4]  * data[2] * data[13] -
            data[12] * data[1] * data[6] +
            data[12] * data[2] * data[5];

    inv[3] = -data[1] * data[6] * data[11] +
            data[1] * data[7] * data[10] +
            data[5] * data[2] * data[11] -
            data[5] * data[3] * data[10] -
            data[9] * data[2] * data[7] +
            data[9] * data[3] * data[6];

    inv[7] = data[0] * data[6] * data[11] -
            data[0] * data[7] * data[10] -
            data[4] * data[2] * data[11] +
            data[4] * data[3] * data[10] +
            data[8] * data[2] * data[7] -
            data[8] * data[3] * data[6];

    inv[11] = -data[0] * data[5] * data[11] +
            data[0] * data[7] * data[9] +
            data[4] * data[1] * data[11] -
            data[4] * data[3] * data[9] -
            data[8] * data[1] * data[7] +
            data[8] * data[3] * data[5];

    inv[15] = data[0] * data[5] * data[10] -
            data[0] * data[6] * data[9] -
            data[4] * data[1] * data[10] +
            data[4] * data[2] * data[9] +
            data[8] * data[1] * data[6] -
            data[8] * data[2] * data[5];

    det = data[0] * inv[0] + data[1] * inv[4] + data[2] * inv[8] + data[3] * inv[12];

    if (det == 0.f)
        return false;

    det = 1.f / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    memcpy(data, invOut, 16*sizeof(GLfloat));

    return true;
}

Matrix4x4 Matrix4x4::calcInverse()
{
    gsl::mat4 mat{*this};
    mat.LU();
    gsl::vec4 params[4];
    for (unsigned int i{0}; i < 4; ++i) {
        float v[4];
        for (unsigned int j{0}; j < 4; ++j)
            v[j] = (i == j) ? 1.f : 0.f;
        params[i] = mat.solve({v[0], v[1], v[2], v[3]});
    }
    return gsl::mat4{{params[0], params[1], params[2], params[3]}};
}

void Matrix4x4::translateX(GLfloat x)
{
    translate(x, 0.f, 0.f);
}

void Matrix4x4::translateY(GLfloat y)
{
    translate(0.f, y, 0.f);
}


void Matrix4x4::translateZ(GLfloat z)
{
    translate(0.f, 0.f, z);
}

void Matrix4x4::setPosition(GLfloat x, GLfloat y, GLfloat z)
{
    data[3] = x;
    data[7] = y;
    data[11] = z;
}

Vector3D Matrix4x4::getPosition()
{
    return gsl::Vector3D(data[3], data[7], data[11]);
}

void Matrix4x4::rotateX(GLfloat degrees)
{
    GLfloat rad = deg2radf(degrees);

    Matrix4x4 temp =
    {
        1.f,   0.f,       0.f,    0.f,
        0.f, std::cos(rad),  -std::sin(rad), 0.f,
        0.f, std::sin(rad), std::cos(rad), 0.f,
        0.f,   0.f,       0.f,    1.f
    };

    *this = (*this)*temp;
}

void Matrix4x4::rotateY(GLfloat degrees)
{
    GLfloat rad = deg2radf(degrees);

    Matrix4x4 temp =
    {
        std::cos(rad), 0.f, std::sin(rad), 0.f,
        0.f,       1.f,      0.f,       0.f,
        -std::sin(rad), 0.f,  std::cos(rad), 0.f,
        0.f,       0.f,      0.f,       1.f
    };

    *this = (*this)*temp;
}

void Matrix4x4::rotateZ(GLfloat degrees)
{
    GLfloat rad = deg2radf(degrees);

    Matrix4x4 temp =
    {
        std::cos(rad),  -std::sin(rad), 0.f, 0.f,
        std::sin(rad),  std::cos(rad), 0.f, 0.f,
        0.f,            0.f,       1.f, 0.f,
        0.f,            0.f,       0.f, 1.f
    };

    *this = (*this)*temp;
}

// Rotate around a given vector
//void Matrix4x4::rotate(GLfloat angle, Vector3D vector)
//{
//    vector.normalize();

    //    https://learnopengl.com/Getting-started/Transformations
    //    cosθ+Rx2(1−cosθ)      RxRy(1−cosθ)−Rzsinθ     RxRz(1−cosθ)+Rysinθ     0
    //    RyRx(1−cosθ)+Rzsinθ   cosθ+Ry2(1−cosθ)        RyRz(1−cosθ)−Rxsinθ     0
    //    RzRx(1−cosθ)−Rysinθ   RzRy(1−cosθ)+Rxsinθ     cosθ+Rz2(1−cosθ)        0
    //    0                     0                       0                       1
//}

//void Matrix4x4::rotate(GLfloat angle, GLfloat xIn, GLfloat yIn, GLfloat zIn)
//{
//    rotate(angle, Vector3D(xIn, yIn, zIn));
//}

void Matrix4x4::scale(Vector3D s)
{
    scale(s.getX(), s.getY(), s.getZ());
}

void Matrix4x4::scale(GLfloat uniformScale)
{
    scale(uniformScale, uniformScale, uniformScale);
}

void Matrix4x4::scale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ)
{
    Matrix4x4 temp =
    {
        scaleX,  0.f,    0.f,   0.f,
        0.f,   scaleY,  0.f,   0.f,
        0.f,    0.f,   scaleZ, 0.f,
        0.f,    0.f,    0.f,   1.f
    };

    *this = (*this)*temp;
}


const GLfloat *Matrix4x4::constData() const
{
    return &data[0];
}

void Matrix4x4::transpose()
{
    std::swap(data[1], data[4]);
    std::swap(data[2], data[8]);
    std::swap(data[3], data[12]);

    std::swap(data[6], data[9]);
    std::swap(data[7], data[13]);
    std::swap(data[11], data[14]);
}

Matrix4x4 Matrix4x4::transposed() const
{
    return gsl::Matrix4x4{
        data[0], data[4], data[8], data[12],
        data[1], data[5], data[9], data[13],
        data[2], data[6], data[10], data[14],
                data[3], data[7], data[11], data[15]};
}

void Matrix4x4::pivot(unsigned int i)
{
    // Search for maximum in this column
    float maxEl = std::abs(at(i, i));
    unsigned int maxRow = i;
    for (unsigned int k{i+1}; k < 4; k++) {
        if (std::abs(at(i, k)) > maxEl) {
            maxEl = std::abs(at(i, k));
            maxRow = k;
        }
    }

    // Swap maximum row with current row (column by column)
    for (unsigned int k{0}; k < 4; k++) { // k needs to start at 0, cause I need to swap the entire row
        std::swap(at(k, maxRow), at(k, i));
    }
    // Swap the augmented part of the matrix. (Actually swap the order, and use that to swap)
    std::swap(pivotOrder[maxRow], pivotOrder[i]);
}

Matrix4x4 Matrix4x4::LU()
{
    // Make a copy of this
    auto mat{*this};

    // Reset from former pivoting
    for (int i{0}; i < 4; i++)
        mat.pivotOrder[i] = i;

    for (unsigned int i{0}; i < 4; i++) {
        mat.pivot(i);

        // Make all rows below this one 0 in current column
        for (unsigned int k{i + 1}; k < 4; k++) {
            float c = mat.at(i, k) / mat.at(i, i);

            if (c != c) { // NaN is the only number who is unequal to itself
                c = 0;
            }
            for (unsigned int j{i}; j < 4; j++) {
                if (j == i) {
                    // at(j, k) = 0; // Set to zero manually to dodge precision errors.
                    mat.at(j, k) = c; // Store c in here for efficiency!
                } else {
                    mat.at(j, k) -= c * mat.at(j, i);
                }
            }
        }
    }

    return mat;
}

Vector4D Matrix4x4::solve(Vector4D b)
{
    // Gjør LU faktorisering og pivotering på b:
    b = gsl::vec4{b[pivotOrder[0]], b[pivotOrder[1]], b[pivotOrder[2]], b[pivotOrder[3]]}; // Pivoting
    for (unsigned int i{0}; i < 4; i++) {
        for (unsigned int k{i + 1}; k < 4; k++) {
            float c = at(i, k);
            // NaN check
            if (c != c) {
                c = 0;
            }
            b[static_cast<int>(k)] -= c * b[static_cast<int>(i)];
        }
    }

    gsl::vec4 x{b};
    for (int i{4 - 1}; i >= 0; i--) {
        // Innsettingsmetoden
        for (int j{i + 1}; j < 4; j++)
            x[i] -= (*this)(j, i) * x[j];
        x[i] /= (*this)(i, i);
    }
    return x;
}

Matrix4x4& Matrix4x4::setOrtho(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat nearPlane, GLfloat farPlane)
{
    return *this =
    {
        2.f/(r-l), 0.f, 0.f, 0.f,
                0.f, 2.f/(t-b), 0.f, 0.f,
                0.f, 0.f, -2.f/(farPlane-nearPlane), 0.f,
                -(r+l)/(r-l), -(t+b)/(t-b), -(farPlane+nearPlane)/(farPlane-nearPlane), 1.f
    };
}

//From Interactive Computer Graphics ch. 5
Matrix4x4& Matrix4x4::setFrustum(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
    return *this =
    {
        2*nearPlane/(right-left),    0.f,                    (right+left)/(right-left),                      0.f,
                0.f,              2*nearPlane/(top-bottom),  (top+bottom)/(top-bottom),                      0.f,
                0.f,                 0.f,                    -(farPlane+nearPlane)/(farPlane-nearPlane),    -2*farPlane*nearPlane/(farPlane-nearPlane),
                0.f,                 0.f,                               -1.0f,                               0.0f
    };
}

Matrix4x4& Matrix4x4::setPersp(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane)
{
    /* General form of the Projection Matrix
    //
    // uh = Cot( fov/2 ) == 1/Tan(fov/2)
    // uw / uh = 1/aspect
    //
    //   uw         0       0       0
    //    0        uh       0       0
    //    0         0      f/(f-n)  1
    //    0         0    -fn/(f-n)  0 */

    //Checking numbers for no division on zero:
    if (fieldOfView <= 0.f)
        fieldOfView = 30.f;
    if (aspectRatio <= 0.f)
        aspectRatio = 1.3f;
    if (farPlane - nearPlane <= 0.f)
    {
        nearPlane = 1.f;
        farPlane = 100.f;
    }

    GLfloat uh = static_cast<float>(1/std::tan(gsl::deg2rad(static_cast<double>(fieldOfView)/2)));
    GLfloat uw = (1/aspectRatio) * uh;

    return *this =
    {
        uw,     0.f,    0.f,                                        0.f,
        0.f,    uh,     0.f,                                        0.f,
        0.f,    0.f,    -(farPlane)/(farPlane-nearPlane),    -2 * farPlane*nearPlane/(farPlane-nearPlane),
        0.f,    0.f,    -1.f,                                        0.f
    };



    /*
           //fieldOfView = verticalAngle
           //Find right, and calculate the rest from there
            GLfloat scale = std::tan(verticalAngle * PI / 360.f) * nearPlane;
            GLfloat r = aspectRatio * scale;
            GLfloat t = scale;

            //Create perspective-frustrum
            *this =
            {
                nearPlane/r, 0.f, 0.f, 0.f,
                0.f, nearPlane/t, 0.f, 0.f,
                0.f, 0.f, -(farPlane+nearPlane)/(farPlane-nearPlane), -2*farPlane*nearPlane/(farPlane-nearPlane),
                0.f, 0.f, -1.f, 0.f
            };
    */
}

Matrix4x4 Matrix4x4::ortho(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat nearPlane, GLfloat farPlane)
{
    return
    {
        2.f/(r-l), 0.f, 0.f, 0.f,
        0.f, 2.f/(t-b), 0.f, 0.f,
        0.f, 0.f, -2.f/(farPlane-nearPlane), 0.f,
        -(r+l)/(r-l), -(t+b)/(t-b), -(farPlane+nearPlane)/(farPlane-nearPlane), 1.f
    };
}

Matrix4x4 Matrix4x4::frustum(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
    return
    {
        2*nearPlane/(right-left),   0.f,                            (right+left)/(right-left),                      0.f,
        0.f,                        2*nearPlane/(top-bottom),       (top+bottom)/(top-bottom),                      0.f,
        0.f,                        0.f,                            -(farPlane+nearPlane)/(farPlane-nearPlane),     -2*farPlane*nearPlane/(farPlane-nearPlane),
        0.f,                        0.f,                            -1.0f,                                          0.0f
    };
}

Matrix4x4 Matrix4x4::persp(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane)
{
    /* General form of the Projection Matrix
    //
    // uh = Cot( fov/2 ) == 1/Tan(fov/2)
    // uw / uh = 1/aspect
    //
    //   uw         0       0       0
    //    0        uh       0       0
    //    0         0      f/(f-n)  1
    //    0         0    -fn/(f-n)  0 */

    //Checking numbers for no division on zero:
    if (fieldOfView <= 0.f)
        fieldOfView = 30.f;
    if (aspectRatio <= 0.f)
        aspectRatio = 1.3f;
    if (farPlane - nearPlane <= 0.f)
    {
        nearPlane = 1.f;
        farPlane = 100.f;
    }

    GLfloat uh = static_cast<float>(1/std::tan(gsl::deg2rad(static_cast<double>(fieldOfView)/2)));
    GLfloat uw = (1/aspectRatio) * uh;

    return
    {
        uw,     0.f,    0.f,                                        0.f,
        0.f,    uh,     0.f,                                        0.f,
        0.f,    0.f,    -(farPlane)/(farPlane-nearPlane),    -2 * farPlane*nearPlane/(farPlane-nearPlane),
        0.f,    0.f,    -1.f,                                        0.f
    };
}

void Matrix4x4::setLookAt(const Vector3D &eye, const Vector3D &center, const Vector3D &up_axis)
{
    Vector3D f = center-eye;    //forward
    f.normalize();
    Vector3D s = Vector3D::cross(f, up_axis);   //sideways
    s.normalize();
    Vector3D u = Vector3D::cross(s, f);     //up

    *this =
    {
        s.getX(),  s.getY(),  s.getZ(), -Vector3D::dot(s, eye),
                u.getX(),  u.getY(),  u.getZ(), -Vector3D::dot(u, eye),
                -f.getX(), -f.getY(), -f.getZ(), Vector3D::dot(f, eye),
                0.f, 0.f, 0.f, 1.f
    };
}

Matrix4x4 Matrix4x4::lookAtRotation(const Vector3D &from, const Vector3D &to, const Vector3D &up_axis)
{
    Vector3D f = (to - from).normalized(); // Forward (n)
    Vector3D r = Vector3D::cross(up_axis, f).normalized(); // Right (u)
    Vector3D u = Vector3D::cross(f, r).normalized(); // Up (v)

    return gsl::Matrix4x4
    {
        r.x, u.x, f.x, 0.f,
        r.y, u.y, f.y, 0.f,
        r.z, u.z, f.z, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
}

Matrix4x4 Matrix4x4::viewMatrix(const Vector3D &from, const Vector3D &to, const Vector3D &up_axis)
{
    return lookAtRotation(from, to, up_axis).transposed() * translation(-from);
}

Matrix4x4 Matrix4x4::modelMatrix(const vec3 &pos, const quat &rot, const vec3 &scale)
{
    return gsl::mat4::translation(pos) * rot.toMat() * gsl::mat4::scaling(scale);
}

std::vector<mat4> Matrix4x4::extractModelMatrix(mat4 model)
{
    // This should be safe to do as the order a model matrix is made is:
    // M = T * R * S
    // Which means that translation is on the left hand side and will only be
    // multiplied by the 1 placed at the lower right end in the rotation matrix.
    // Thanks affine rooms
    gsl::mat4 T{1.f};
    for (int i{0}; i < 3; ++i)
        std::swap(T(i, 3), model(i, 3));

    // Find scale before rotation because scale is multiplied column wise
    // into all the rows of the rotation matrix.
    // To extract the scale we just take the magnitude of each column vector
    // Note: Non-uniform scaling might not make the rotation matrix correct
    gsl::mat4 S{1.f};
    for (int i{0}; i < 3; ++i)
        S(i, i) = gsl::vec3{model(0, i), model(1, i), model(2, i)}.length();

    // Divide remaining matrix by scale.
    for (int x{0}; x < 3; ++x)
        for (int y{0}; y < 3; ++y)
            model(y, x) /= S(x, x);

    // Lastly extract rotation matrix
    // This should basically just be all that's left
    gsl::mat4 R{model};

    return std::vector<gsl::mat4>{T, S, R};
}

void Matrix4x4::setRotationToVector(const Vector3D &direction, Vector3D up)
{
    Vector3D xaxis = Vector3D::cross(up, direction);
    xaxis.normalize();

    Vector3D yaxis = Vector3D::cross(direction, xaxis);
    yaxis.normalize();

    data[0] = xaxis.x;
    data[1] = yaxis.x;
    data[2] = direction.x;

    data[4] = xaxis.y;
    data[5] = yaxis.y;
    data[6] = direction.y;

    data[8] = xaxis.z;
    data[9] = yaxis.z;
    data[10] = direction.z;
}

void Matrix4x4::translate(GLfloat x, GLfloat y, GLfloat z)
{
    Matrix4x4 mat =
    {
        1.f, 0.f, 0.f, x,
        0.f, 1.f, 0.f, y,
        0.f, 0.f, 1.f, z,
        0.f, 0.f, 0.f, 1.f
    };

    *this = (*this)*mat;
}

void Matrix4x4::translate(Vector3D positionIn)
{
    Matrix4x4 mat =
    {
        1.f, 0.f, 0.f, positionIn.getX(),
        0.f, 1.f, 0.f, positionIn.getY(),
        0.f, 0.f, 1.f, positionIn.getZ(),
        0.f, 0.f, 0.f, 1.f
    };

    *this = (*this)*mat;
}

Matrix4x4 Matrix4x4::translation(const Vector3D &trans)
{
    return gsl::Matrix4x4
    {
        1.f, 0.f, 0.f, trans.x,
        0.f, 1.f, 0.f, trans.y,
        0.f, 0.f, 1.f, trans.z,
        0.f, 0.f, 0.f, 1.f
    };
}

Matrix4x4 Matrix4x4::scaling(const Vector3D &scale)
{
    return gsl::mat4
    {
        scale.x,    0.f,    0.f,    0.f,
        0.f,    scale.y,    0.f,    0.f,
        0.f,    0.f,    scale.z,    0.f,
        0.f,    0.f,    0.f,    1.f,
    };
}

Matrix2x2 Matrix4x4::toMatrix2()
{
    return
    {
        data[0], data[1],
                data[4], data[5]
    };
}

Matrix3x3 Matrix4x4::toMatrix3() const
{
    return
    {
        data[0], data[1], data[2],
                data[4], data[5], data[6],
                data[8], data[9], data[10]
    };
}

GLfloat &Matrix4x4::at(unsigned int y, unsigned int x)
{
    return data[y * 4 + x];
}

GLfloat Matrix4x4::at(unsigned int y, unsigned int x) const
{
    return data[y * 4 + x];
}

GLfloat& Matrix4x4::operator()(const int &y, const int &x)
{
    return data[y * 4 + x];
}

GLfloat Matrix4x4::operator()(const int &y, const int &x) const
{
    return data[y * 4 + x];
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &other) const
{
    return
    {
        data[0]  * other.data[0] + data[1]  * other.data[4] + data[2]  * other.data[8]  + data[3]  * other.data[12],
                data[0]  * other.data[1] + data[1]  * other.data[5] + data[2]  * other.data[9]  + data[3]  * other.data[13],
                data[0]  * other.data[2] + data[1]  * other.data[6] + data[2]  * other.data[10] + data[3]  * other.data[14],
                data[0]  * other.data[3] + data[1]  * other.data[7] + data[2]  * other.data[11] + data[3]  * other.data[15],

                data[4]  * other.data[0] + data[5]  * other.data[4] + data[6]  * other.data[8]  + data[7]  * other.data[12],
                data[4]  * other.data[1] + data[5]  * other.data[5] + data[6]  * other.data[9]  + data[7]  * other.data[13],
                data[4]  * other.data[2] + data[5]  * other.data[6] + data[6]  * other.data[10] + data[7]  * other.data[14],
                data[4]  * other.data[3] + data[5]  * other.data[7] + data[6]  * other.data[11] + data[7]  * other.data[15],

                data[8]  * other.data[0] + data[9]  * other.data[4] + data[10] * other.data[8]  + data[11] * other.data[12],
                data[8]  * other.data[1] + data[9]  * other.data[5] + data[10] * other.data[9]  + data[11] * other.data[13],
                data[8]  * other.data[2] + data[9]  * other.data[6] + data[10] * other.data[10] + data[11] * other.data[14],
                data[8]  * other.data[3] + data[9]  * other.data[7] + data[10] * other.data[11] + data[11] * other.data[15],

                data[12] * other.data[0] + data[13] * other.data[4] + data[14] * other.data[8]  + data[15] * other.data[12],
                data[12] * other.data[1] + data[13] * other.data[5] + data[14] * other.data[9]  + data[15] * other.data[13],
                data[12] * other.data[2] + data[13] * other.data[6] + data[14] * other.data[10] + data[15] * other.data[14],
                data[12] * other.data[3] + data[13] * other.data[7] + data[14] * other.data[11] + data[15] * other.data[15]
    };
}

GLfloat Matrix4x4::getFloat(int space)
{
    return data[space];
}

Vector4D Matrix4x4::operator*(const Vector4D &v)
{
    return Vector4D(data[0]*v.getX()  + data[1]*v.getY()  + data[2]*v.getZ()  + data[3] *v.getW(),
            data[4]*v.getX()  + data[5]*v.getY()  + data[6]*v.getZ()  + data[7] *v.getW(),
            data[8]*v.getX()  + data[9]*v.getY()  + data[10]*v.getZ() + data[11] *v.getW(),
            data[12]*v.getX() + data[13]*v.getY() + data[14]*v.getZ() + data[15] *v.getW());
}

} //namespace
