#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "gltypes.h"
#include "mathfwd.h"
#include <cmath>
#include <iostream>
#include <QDebug>

#include <QJsonArray>

class QVector3D;


namespace gsl
{

/** A mathematical 3 dimensional vector.
 * @brief A mathematical 3 dimensional vector.
 */
class Vector3D
{
public:
    //Constructors
    Vector3D(GLfloat x_in = 0.f, GLfloat y_in = 0.f, GLfloat z_in = 0.f);
    Vector3D(const int v);
    Vector3D(const double v);
    Vector3D(const gsl::ivec3& v);

    //Operators
    const Vector3D& operator= (const Vector3D &rhs);     // v = v
    Vector3D operator+ (const Vector3D &rhs) const;      // v + v
    Vector3D operator- (const Vector3D &rhs) const;      // v - v
    Vector3D& operator+= (const Vector3D &rhs);          // v += v
    Vector3D& operator-= (const Vector3D &rhs);          // v -= v
    Vector3D operator- () const;                     // -v
    Vector3D operator* (GLfloat rhs) const;          // v * f
    Vector3D operator/ (GLfloat rhs) const;
    friend Vector3D operator* (GLfloat f, const Vector3D &rhs); // f * v
    friend Vector3D operator/ (GLfloat f, const Vector3D &rhs);
    GLfloat operator* (const Vector3D &rhs) const; // v * v - dot product
    Vector3D operator^ (const Vector3D& rhs) const; // v x v  - cross product

    GLfloat& operator[](const int index);
    GLfloat operator[] (const int index) const;

    //Functions
    GLfloat length() const;
    void normalize();
    Vector3D normalized() const;
    static Vector3D cross(const Vector3D &v1, const Vector3D &v2);
    static GLfloat dot(const Vector3D &v1, const Vector3D &v2);
    Vector3D project(const Vector3D &b) const;
    static Vector3D project (const Vector3D& a, const Vector3D& b);
    bool isZero() const;

    void rotateX(GLfloat angle);
    void rotateY(GLfloat angle);
    void rotateZ(GLfloat angle);

    Quaternion toQuat() const;

    //Getters and setters
    GLfloat getX() const;
    void setX(const GLfloat &value);

    GLfloat getY() const;
    void setY(const GLfloat &value);

    GLfloat getZ() const;
    void setZ(const GLfloat &value);

    GLfloat *xP();
    const GLfloat *xP() const;
    GLfloat *yP();
    GLfloat *zP();

    GLfloat* data() { return &x; }

    QJsonArray toJSON();
    void fromJSON(const QJsonArray &array);


    //Friend functions
    friend std::ostream& operator<<(std::ostream &output, const Vector3D &rhs )
    {
        output << "X = " << rhs.x << ", Y = " << rhs.y << ", Z = " << rhs.z;
        return output;
    }

    //OEF: Adding these again to be able to debug easier. cout does not print until program is finished!
    friend QDebug operator<<(QDebug out, const Vector3D &rhs)
    {
        out << "(" << rhs.x << "," << rhs.y << "," << rhs.z << ")";
        return  out;
    }        //    QVector3D getQVector() const;   //for use with qDebug()

    QVector3D getQVector() const;   //for use with qDebug()

    //private:  //Making these public to avoid having to do things like setY(getY()+3) etc.
    GLfloat x;
    GLfloat y;
    GLfloat z;
};



/** Integer vector3.
 * @brief Integer vector3.
 */
class IVector3D
{
public:
    IVector3D(GLint x_in = 0, GLint y_in = 0, GLint z_in = 0);

    gsl::ivec3 operator+ (const gsl::ivec3& rhs) const;
    gsl::ivec3 operator- (const gsl::ivec3& rhs) const;
    gsl::ivec3 operator* (const gsl::ivec3& rhs) const;
    gsl::ivec3 operator/ (const gsl::ivec3& rhs) const;
    bool operator== (const gsl::ivec3& rhs) const;

    GLint x;
    GLint y;
    GLint z;
};

} //namespace

#endif // VECTOR3D_H
