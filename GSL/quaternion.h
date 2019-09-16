#ifndef QUATERNION_H
#define QUATERNION_H

#include "gltypes.h"
#include "mathfwd.h"
#include "vector3d.h"

/** Quaternions
A mathematical structure that uses a complex numnber
 * in a higher body that can be used to efficiently and
 * easy rotate points in 3D space.
 * A quaternion is defined by
 * q = s * v
 * where s is a scalar and v is a 3d vector.
 * q can also be written as
 * q = s + ai + bj + ck
 * where i, j and k are the unit vectors.
 * A quaternion can be defined programmically as a vector4d
 * where the first (or last) of the elements are the scalar
 * part of the quaternion. In this quaternion the first
 * element is the scalar part of the quaternion:
 * q = [s, x, y, z]
 *
 * @brief quaternion. Defines rotation in 3d space.
 * @see https://en.wikipedia.org/wiki/Quaternion
 * @author andesyv (Quaternion concept described by Sir William Rowan Hamilton)
 */

namespace gsl
{

class Quaternion
{
public:
    // Another way to visualize quaternions
    struct Pair
    {
        Pair(GLfloat scalar = 1.f, const gsl::vec3& vector = gsl::vec3{0.f, 0.f, 0.f});

        GLfloat s;
        gsl::vec3 v;
    };

public:
    Quaternion(GLfloat sIn = 0.f, GLfloat iIn = 0.f, GLfloat jIn = 0.f, GLfloat kIn = 0.f);
    Quaternion(GLfloat scalar, const gsl::vec3& v);
    Quaternion(const Pair& pair);



    // Mathematical operations
    gsl::quat operator+ (const gsl::quat& quat) const;
    gsl::quat operator+ (GLfloat scalar) const;
    friend gsl::quat operator+ (GLfloat scalar, const gsl::quat& quat);

    gsl::quat operator- (const gsl::quat& quat) const;
    gsl::quat operator- (GLfloat scalar) const;
    friend gsl::quat operator- (GLfloat scalar, const gsl::quat& quat);

    gsl::quat operator* (const gsl::quat& quat) const;
    gsl::quat operator* (GLfloat scalar) const;
    friend gsl::quat operator* (GLfloat scalar, const gsl::quat& quat);

    gsl::quat& operator= (const gsl::quat& quat);
    gsl::quat& operator+= (const gsl::quat& quat);
    gsl::quat& operator+= (GLfloat scalar);
    gsl::quat& operator-= (const gsl::quat& quat);
    gsl::quat& operator-= (GLfloat scalar);
    gsl::quat& operator*= (const gsl::quat& quat);
    gsl::quat& operator*= (GLfloat scalar);




    // Mathematical functions
    /** Creates a quaternion used to rotate specified angles around a given axis
     * Constructs a quaternion where s = cos(angle) / 2
     * and v = sin(angle) / 2
     * If the axis is a unit vector, rotatePointUnit can be used to rotate,
     * otherwise rotatePoint must be used.
     * @param angle - The angle around the given axis to rotate
     * @param axis - The axis to rotate around
     * @brief Create rotation quaternion
     */
    static gsl::quat rot(GLfloat angle, const gsl::vec3& axis);
    /** Uses a quaternion to rotate a point
     * Performs the operation:
     * P' = q * P * q^-1
     * where P is the quaternion (0, p) constructed
     * from the point p.
     * Slightly slower than rotatePointUnit but can
     * be used with any quaternion.
     * @brief Rotate point with quaternion
     * @param p - The point to transform
     * @param rot - The quaternion used to perform the rotation
     * @return The rotated point.
     */
    static gsl::vec3 rotatePoint(const gsl::vec3& p, const gsl::quat& rot);
    /** Uses a quaternion to rotate a point.
     * Similar to rotatePoint, but doesn't divide by the magnitude of
     * the quaternion, resulting in a wrong position unless the vector
     * used to make the quaternion was normalized.
     * @brief Rotate point with unit quaternion
     * @param p - The point to transform
     * @param rot - The quaternion from a unit vector used to perform the rotation
     * @return The rotated point.
     */
    static gsl::vec3 rotatePointUnit(const gsl::vec3& p, const gsl::quat& rot);
    /** Converts the quaternion to a rotation matrix.
     * @brief Converts quaternion to a rotation matrix
     * @return A mat4 that does the same operation as
     * rotating with the quaternion
     */
    gsl::mat4 toMat() const;
    /** Converts the quaternion to normal
     * euler angles.
     * Angles is specified in radians
     * @see https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_Angles_Conversion
     * @brief Convert to euler angles
     * @return vec3 corresponding to angles rotation around
     * x-axis, y-axis and z-axis.
     */
    gsl::vec3 toEuler() const;
    /// The conjugate of the quaternion
    gsl::quat conj() const;
    /** Returns the squared magnitude of the
     * quaternion.
     * |q|^2 = s^2 + v * v
     * @brief Magnitude squared
     * @return
     */
    GLfloat sizeSqrd() const;
    /// The inverse of the quaternion
    gsl::quat inverse() const;



    // Helper functions
    /** Places the contents of a quaternion
     * into a vec4.
     * @brief Converts quaternion to a vec4
     * @return vec4 with same data as quaternion
     */
    gsl::vec4 toVec4() const;
    /** Converts the quaternion to a quaternion pair
     * which is just another way of using a quaternion.
     * Quaternion pair is:
     * (s, v) = s + ai + bj + ck, v = [a, b, c]
     * @brief Convert quaternion to a quaternion pair
     * @return A quaternion pair
     */
    gsl::quat::Pair toPair() const;



    // Data:
    GLfloat s;
    GLfloat i;
    GLfloat j;
    GLfloat k;
};

}

#endif // QUATERNION_H
