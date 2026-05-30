#ifndef COLLIDER_HXX
#define COLLIDER_HXX

#include "../../math/headers/vec3.hxx"
#include "../../math/headers/mat3.hxx"

enum class ColliderType{
    Plane,
    Sphere,
    Box
};

class Collider
{
public:
    Collider()  {};
    ~Collider() {}

    virtual Mat3x3       inertiaTensor() const = 0;
    virtual Vector3f     centerOfMass()  const = 0;
    virtual ColliderType getType()       const = 0;
protected:
    Vector3f m_centerOfMass = Vector3f(0.0);

};

class SphereCollider : public Collider
{
public:
    SphereCollider() = default;
    SphereCollider(float radius) : m_radius(radius) {}

    Vector3f centerOfMass() const override
    {
        return m_centerOfMass;
    }

    Mat3x3 inertiaTensor() const override
    {
        float i = (2.0f / 5.0f) * m_radius * m_radius;

        return Mat3x3(
            Vector3f(i, 0.0, 0.0),
            Vector3f(0.0, i, 0.0),
            Vector3f(0.0, 0.0, i)
        );
    }

    ColliderType getType() const override { return ColliderType::Sphere; }

    float radius() const { return m_radius; }

private:
    float m_radius = 0; 
};

#endif