#include "headers/rigidbody.hxx"

Vector3f RigidBody::getCenterOfMassLocal() const
{
    return collider->centerOfMass();
}

Vector3f RigidBody::getCenterOfMassWorld() const 
{
    const Vector3f localCOM = getCenterOfMassLocal();
    return position() + orientation() * localCOM;
}

Vector3f RigidBody::WorldToLocal(const Vector3f &point) const
{
    return orientation().inverse() * (point - getCenterOfMassWorld());
}

Vector3f RigidBody::LocalToWorld(const Vector3f &point) const
{
    return getCenterOfMassWorld() + orientation() * point;
}

void RigidBody::applyLinearImpulse(const Vector3f &impulse)
{
    if (!isStatic())
    {
        velocity += impulse * inverseMass();
    }
}

Mat3x3 RigidBody::getWorldInvIntertiaTensor() const
{
    Mat3x3 r = orientation().toMat3x3();
    return r * getInverseInertiaTensor() * r.transpose();
}