#ifndef RIGIDBODY_HXX
#define RIGIDBODY_HXX

#include "../../math/headers/math.hxx"
#include "collider.hxx"
#include <memory>

#define MAX_ANG_VEL 30.0f

struct RigidBody
{
    RigidBody(std::unique_ptr<Collider> collider) : collider(std::move(collider)) {}

    float     mass = 1.0f;
    float     restitution = 0.5f;
    float     linearDamping = 0.5f;

    Transform transform;
    Vector3f  velocity;
    Vector3f  rotation;

    std::unique_ptr<Collider> collider;

    Vector3f getCenterOfMassWorld() const;
    Vector3f getCenterOfMassLocal() const;

    Vector3f WorldToLocal(const Vector3f &point) const;
    Vector3f LocalToWorld(const Vector3f &point) const;

    void applyLinearImpulse(const Vector3f &impulse);

    Mat3x3 getWorldInvIntertiaTensor() const;

    Vector3f position() const
    {
        return transform.translation;
    }

    Quat orientation() const
    {
        return transform.orientation;
    }

    void translate(const Vector3f &delta)
    {
        transform.translation += delta;
    }

    void rotate(const Quat &delta)
    {
        transform.orientation = delta * transform.orientation;
    }

    const Mat3x3 getInverseInertiaTensor() const
    {
        return collider->inertiaTensor().inverse();
    }

    bool isStatic() const
    {
        return std::isinf(mass);
    }

    void makeStatic()
    {
        mass = INFINITY;
    }

    float inverseMass() const
    {
        return isStatic() ? 0.0f : 1.0f / mass;
    }
};

#endif
