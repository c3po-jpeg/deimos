#include "headers/rigidbody.hxx"

Vector3f RigidBody::getCenterOfMassLocal() const
{
    return collider->centerOfMass();
}

Vector3f RigidBody::getCenterOfMassWorld() const
{
    const Vector3f localCOM = getCenterOfMassLocal();
    return position() + orientation().rotatePoint(localCOM);
}

Vector3f RigidBody::WorldToLocal(const Vector3f &point) const
{
    return orientation().inverse().rotatePoint(point - getCenterOfMassWorld());
}

Vector3f RigidBody::LocalToWorld(const Vector3f &point) const
{
    return getCenterOfMassWorld() + orientation().rotatePoint(point);
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

void RigidBody::applyLinearImpulse(const Vector3f &impulse)
{
    velocity += impulse * inverseMass();
}

void RigidBody::applyAngularImpulse(const Vector3f &impulse)
{
    angularVelocity += getWorldInvInertiaTensor() * impulse;

    if (angularVelocity.magSqrd() > MAX_ANG_VEL * MAX_ANG_VEL)
    {
        angularVelocity = angularVelocity.normalize() * MAX_ANG_VEL;
    }
}

void RigidBody::applyImpulseAtPoint(const Vector3f &impulse, const Vector3f &contactPoint)
{
    applyLinearImpulse(impulse);

    Vector3f pos = getCenterOfMassWorld();

    Vector3f r = contactPoint - pos;
    Vector3f dL = cross(r, impulse);
    applyAngularImpulse(dL);
}

void RigidBody::update(float dt)
{
    transform.translation += velocity * dt;

    Vector3f centerOfMass = getCenterOfMassWorld();
    Vector3f cmToPosition = position() - centerOfMass;

    Mat3x3 r = orientation().toMat3x3();
    Mat3x3 inertiaTensor = r * collider->inertiaTensor() * r.transpose();
    Vector3f alpha = inertiaTensor.inverse() * cross(angularVelocity, inertiaTensor * angularVelocity);
    angularVelocity += alpha * dt;

    Vector3f dAngle = angularVelocity * dt;
    float angle = dAngle.mag();
    Quat dq = (angle > 1e-8f) ? Quat::fromRadians(angle, dAngle) : Quat();

    transform.orientation = (dq * transform.orientation).unit();
    transform.translation = centerOfMass + dq.rotatePoint(cmToPosition);
}