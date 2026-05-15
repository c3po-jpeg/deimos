#ifndef COLLIDER_HXX
#define COLLIDER_HXX

#include "../../math/headers/vec3.hxx"
#include "../../math/headers/mat3.hxx"

class Collider
{
public:
    Collider() {}
    ~Collider() {}

    virtual Mat3x3   inertiaTensor() = 0;
    virtual Vector3f centerOfMass()  = 0;
};

class SphereCollider : Collider
{
public:
    SphereCollider() = default;
    SphereCollider(float radius) : m_radius(radius) {}

    Vector3f centerOfMass() override
    {
        return Vector3f(0.0);
    }

    Mat3x3 inertiaTensor() override
    {
        float i = (2.0f / 5.0f) * m_radius * m_radius;

        return Mat3x3(
            Vector3f(i, 0.0, 0.0),
            Vector3f(0.0, i, 0.0),
            Vector3f(0.0, 0.0, 1)
        );
    }

    float radius() const { return m_radius; }

private:
    float m_radius = 0; 
};

#endif