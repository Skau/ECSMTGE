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

/** Matrix 4x4.
 * 2 dimensional floating point matrix. 4 * 4 version.
 * @brief Matrix 4x4.
 */
class Matrix4x4
{
private:
    GLfloat data[16];

private:

    // Order of pivoting. Used in pivot(uint) and solve(vec4)
    int pivotOrder[4]{0, 1, 2, 3};
    void pivot(unsigned int i);

public:
    // Default constructor. Initializes the matrix with a diagonal at the identity axis.
    Matrix4x4(GLfloat identity);
    Matrix4x4(const gsl::Vector4D& diagonal = gsl::Vector4D{1.f, 1.f, 1.f, 1.f});
    Matrix4x4(std::initializer_list<GLfloat> values);
    explicit Matrix4x4(const std::array<gsl::Vector4D, 4>& vectors);



    // ---------------------- Operators -----------------------------------
    GLfloat& at(unsigned int y, unsigned int x);
    GLfloat at(unsigned int y, unsigned int x) const;
    GLfloat& operator()(const int &y, const int &x);
    GLfloat operator()(const int &y, const int &x) const;

    Matrix4x4 operator*(const Matrix4x4 &other) const;

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


    // ----------------------- Mathematical functions -----------------------
    Matrix4x4 identity() const;
    void setToIdentity();

    // Inverses the matrix.
    bool inverse();
    /** Calculates and returns the inverse matrix.
     *
     * NB: Rounding errors are bound to occour,
     * because I am using floating point numbers.
     * But the error is so small that it should be negligible.
     *
     * @brief Returns the inverse of the matrix
     * @return The inverse matrix
     */
    Matrix4x4 calcInverse();

    void transpose();
    Matrix4x4 transposed() const;

    /** LU() factorizes the set of linear equations into an upper
     * triangular matrix using the Carl Friedrich Gauss' eliminationc++
     * method.
     *
     * Pivot rearranges the matrix in order of the biggest rows (biggest most
     * left element). Pivoting is used by LU decomposition.
     * Before the use of Solve, the matrix has to be LU-decomposed.
     * @see solve(Vector4d &b);
     * @see https://en.wikipedia.org/wiki/LU_decomposition
     */
    Matrix4x4 LU();

    /** Løser Ax=b hvis A er et ligningsett med max
     * 3 ukjente og 4 likningssett, b er svaret på
     * likningsettene og det(A) != 0.
     * NB: Forutsetter at LU faktorisering har blitt
     * gjort på matrisen.
     *
     * Utførelsen av hele prosessen (med LU faktorisering)
     * bruken Gauss eliminisjon til å gjøre matrisen om til
     * en øvre triangular matrise, og utfører så innsettings
     * metoden for å løse likningsettet.
     * @brief Solve a set of linear equations. LU factorization
     * needs to be performed before calculation.
     * @param b = Svaret til likningsettet Ax=b i form
     * av en vector.
     * @see LU();
     * @return En Vector4d hvor x = A^(-1)b.
     */
    gsl::Vector4D solve(Vector4D b);

    // TODO: Switch the gauss calculation from a column wise to a row wise matrix.




    // Helper functions
    void translateX(GLfloat x = 0.f);
    void translateY(GLfloat y = 0.f);
    void translateZ(GLfloat z = 0.f);

    void setPosition(GLfloat x = 0.f, GLfloat y = 0.f, GLfloat z = 0.f);
    gsl::Vector3D getPosition() const;
    GLfloat getFloat(int space);

    void translate(GLfloat x = 0.f, GLfloat y = 0.f, GLfloat z = 0.f);
    void translate(Vector3D positionIn);

    //Rotate using EulerMatrix
    void rotateX(GLfloat degrees = 0.f);
    void rotateY(GLfloat degrees = 0.f);
    void rotateZ(GLfloat degrees = 0.f);
//    void rotate(GLfloat angle, Vector3D vector);
//    void rotate(GLfloat angle, GLfloat xIn, GLfloat yIn, GLfloat zIn);

    void scale(Vector3D s);
    void scale(GLfloat uniformScale);
    void scale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);

    Matrix2x2 toMatrix2();
    Matrix3x3 toMatrix3() const;

    const GLfloat* constData() const;





    // -------------------------- Matrix constructions ----------------------------------------------
    Matrix4x4& setOrtho(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat nearPlane, GLfloat farPlane);
    Matrix4x4& setFrustum(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    Matrix4x4& setPersp(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane);

    static Matrix4x4 ortho(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat nearPlane, GLfloat farPlane);
    static Matrix4x4 frustum(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    static Matrix4x4 persp(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane);
    static Matrix4x4 perspInv(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane);

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

    // Constructs a modelmatrix from the given pos, rot and scale
    static Matrix4x4 modelMatrix(const gsl::vec3& pos, const gsl::quat& rot, const gsl::vec3& scale);
    static Matrix4x4 modelMatrixInv(const gsl::vec3& pos, const gsl::quat& rot, const gsl::vec3& scale);
    static Matrix4x4 viewMatrix(const gsl::quat& rot, const gsl::vec3& pos);
    static Matrix4x4 viewMatrixInv(const gsl::quat& rot, const gsl::vec3& pos);
    // Attempts to exctract the translation, rotation and scale matrix from the combined model matrix.
    static std::vector<gsl::mat4> extractModelMatrix(gsl::mat4 model);

    void setRotationToVector(const Vector3D &direction, Vector3D up = Vector3D(0.f,1.f,0.f));

    static Matrix4x4 translation(const Vector3D& trans);
    static Matrix4x4 scaling(const Vector3D& scale);
};

} //namespace

#endif // MATRIX4X4_H
