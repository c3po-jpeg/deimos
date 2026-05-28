#include "headers/rigidbody.hxx"

void RigidBody::calcDerivedData()
{
    m_transform.orientation = m_orientation.unit();
    m_transform.translation = m_position;
    m_transform.scaling     = Vector3f(1.0f);
}

void RigidBody::addForce(const Vector3f &force)
{
    m_forceAccum += force;
    m_isAwake = true;
}

void RigidBody::integrate(float dt)
{
    clearAccumulators();
}

void RigidBody::getPointInLocalSpace(const Vector3f &point, Vector3f &result) const
{
    Vector3f pt = point - m_position;
    result = m_transform.orientation.conjugate().toMat3x3() * pt;
}

Vector3f RigidBody::getPointInWorldSpace(const Vector3f &point) const
{
    return m_transform.orientation.toMat3x3() * point + m_position;
}

void RigidBody::applyForceAtBodyPoint(const Vector3f &force, const Vector3f &point)
{
    Vector3f pt = getPointInWorldSpace(point);
    applyForceAtPoint(force, pt);

    m_isAwake = true;
}