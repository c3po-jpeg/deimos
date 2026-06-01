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

Mat3x3 RigidBody::getLocalInvInertiaTensor() const
{
    return collider->inertiaTensor().inverse() * inverseMass();
}

Mat3x3 RigidBody::getWorldInvInertiaTensor() const
{
    Mat3x3 r = orientation().toMat3x3();
    return r * getLocalInvInertiaTensor() * r.transpose();
}

void RigidBody::applyAngularImpulse(const Vector3f &impulse)
{
    if (isStatic())
        return;
    
    angularVelocity += getWorldInvInertiaTensor() * impulse;

    if (angularVelocity.magSqrd() > MAX_ANG_VEL * MAX_ANG_VEL)
    {
        angularVelocity = angularVelocity.unit() * MAX_ANG_VEL;
    }
}

void RigidBody::applyImpulseAtPoint(const Vector3f &impulse, const Vector3f & contactPoint)
{
    if(isStatic())
        return;

    applyLinearImpulse(impulse);

    Vector3f pos = getCenterOfMassWorld();

    Vector3f r = contactPoint - pos;
    Vector3f dL = cross(r, impulse);
    applyAngularImpulse(dL);
}

void RigidBody::update(float dt)
{
    if (isStatic())
        return;

    translate(velocity * dt);

    Vector3f centerOfMass = getCenterOfMassWorld();
    Vector3f cmToPosition = position() - centerOfMass;
}