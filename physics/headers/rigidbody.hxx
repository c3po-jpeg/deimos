#ifndef RIGIDBODY_HXX
#define RIGIDBODY_HXX

#include "../../math/headers/math.hxx"
#include "collider.hxx"
#include <memory>

#define MAX_ANG_VEL 30.0f

class RigidBody
{
public: 
    RigidBody(std::unique_ptr<Collider> collider) : m_collider(std::move(collider)) {}

    RigidBody(const RigidBody &)            = delete;
    RigidBody &operator=(const RigidBody &) = delete;

    void calcDerivedData();

    const Mat3x3 getWorldInvIntertiaTensor() const
    {
        Mat3x3 r = m_transform.orientation.toMat3x3();
        return r * getInverseInertiaTensor() * r.transpose();
    }

    const Mat3x3 getInverseInertiaTensor() const
    {
        return m_collider->inertiaTensor().inverse();
    }

    void addForce(const Vector3f &force);

    void clearAccumulators()
    {
        m_forceAccum  = Vector3f(0.0f);
        m_torqueAccum = Vector3f(0.0f);
    }

    bool isStatic() const
    {
        return m_mass == INFINITY;
    }

    void integrate(float dt);

    void applyForceAtPoint(const Vector3f &force, const Vector3f &point);
    void applyForceAtBodyPoint(const Vector3f &force, const Vector3f &point);

    void getPointInLocalSpace(const Vector3f &point, Vector3f &result) const;
    Vector3f getPointInWorldSpace(const Vector3f &point) const;

protected:
    float     m_mass = 1.0f;

    float     m_linearDamping = 0.5f;

    Vector3f  m_position;
    Quat      m_orientation;
    Vector3f  m_velocity;
    Vector3f  m_rotation;
    Vector3f  m_forceAccum;
    Vector3f  m_torqueAccum;
    bool      m_isAwake;

    Transform m_transform;

    std::unique_ptr<Collider> m_collider;
};

#endif
