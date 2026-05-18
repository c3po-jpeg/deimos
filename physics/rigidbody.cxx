#include "headers/rigidbody.hxx"

Mat3x3 RigidBody::getInvInertiaTensor() const
{
    collider->inertiaTensor().inverse() * invMass();
}

Mat3x3 RigidBody::getWorldInvInertiaTesnsor() const
{
    Mat3x3 r = orientation.toMat3x3();
    return r * getInvInertiaTensor() * r.transpose();
}

Vector3f RigidBody::centerOfMass() const
{
    return collider->centerOfMass();
}

Vector3f RigidBody::centerOfMassWorld() const
{
    return position + orientation * centerOfMass();
}

void RigidBody::makeStatic()
{
    mass = INFINITY;
}

void RigidBody::applyLinearImpulse(Vector3f impulse)
{
    // J = dv*m
    // dv= J * m^-1
    linearVelocity += impulse * invMass();
}

void RigidBody::applyAngularImpulse(Vector3f impulse)
{
    angularVelocity += getWorldInvInertiaTesnsor() * impulse;

    if (angularVelocity.magSqrd() > (MAX_ANG_VEL * MAX_ANG_VEL))
    {
        angularVelocity = angularVelocity.unit() * MAX_ANG_VEL;
    }
}

void RigidBody::applyImpulseAtPoint(Vector3f impulse, Point3f point)
{
    applyLinearImpulse(impulse);
    Vector3f r = point - centerOfMassWorld();
    applyAngularImpulse(cross(r, impulse));
}

void RigidBody::update(float dt)
{
    if (isStatic())
        return;

    position        += linearVelocity * dt;
    Vector3f cmToPos = position - centerOfMassWorld();

    Mat3x3 r             = orientation.toMat3x3();
    Mat3x3 inertia_world = r * collider->inertiaTensor() * r.transpose();
    Vector3f alpha       = inertia_world.inverse() * cross(angularVelocity, inertia_world * angularVelocity);
    angularVelocity     += alpha * dt;

    Vector3f dAngle = angularVelocity * dt;
    float angle     = dAngle.mag();
    Quat dq         = angle > 1e-8 ? Quat(to_degrees(angle), dAngle) : Quat();

    orientation = (dq * orientation).unit();
    position = centerOfMassWorld() + dq * cmToPos;
}
