#include "headers/collision.hxx"
#include "headers/rigidbody.hxx"
#include "headers/collider.hxx"

Contact testSphereSphere(RigidBody *a, RigidBody *b)
{
    Contact c;

    const Vector3f ab = b->position() - a->position();
    const float rA = dynamic_cast<SphereCollider *>(a->collider.get())->radius(); // Assuming radius is stored in x component
    const float rB = dynamic_cast<SphereCollider *>(b->collider.get())->radius(); // Assuming radius is stored in x component

    const float rab = rA + rB;
    if ( ab.magSqrd() < rab * rab )
    {
        c.hasCollision = true;
        c.bodyA = a;
        c.bodyB = b;
        c.normal = ab.normalize();
        c.pointAWorldSpace = a->position() + c.normal * rA;
        c.pointBWorldSpace = b->position() - c.normal * rB;
        c.penetrationDepth = rab - ab.mag();
    }
    return c;
}

Contact testCollision(RigidBody *a, RigidBody *b)
{
    switch (a->collider->getType())
    {
    case ColliderType::Sphere:
    {
        switch (b->collider->getType())
        {
        case ColliderType::Sphere:
            return testSphereSphere(a, b);
            break;
        default:
            return Contact();
        }
    }
    default:
        return Contact();
    }

    return Contact();
}

bool RaySpphere(const Vector3f &rayStart, const Vector3f &rayDir, const Vector3f &sphereCenter, const float sphereRadius, float& t1, float &t2)
{
    const Vector3f m = sphereCenter - rayStart;
    const float a = dot(rayDir, rayDir);
    const float b = dot(m, rayDir);
    const float c = dot(m, m) - sphereRadius * sphereRadius;

    const float delta = b * b - a * c;
    const float invA = 1.0f / a;
    if (delta < 0)
    {
        return false;
    }

    const float deltaRoot = sqrt(delta);
    t1 = invA * (b - deltaRoot);
    t2 = invA * (b + deltaRoot);
    
    return true;
}

void resolveCollision(Contact &contact)
{
    RigidBody *bodyA = contact.bodyA;
    RigidBody *bodyB = contact.bodyB;

    const Vector3f ptOnA = contact.pointAWorldSpace;
    const Vector3f ptOnB = contact.pointBWorldSpace;

    const float totalInverseMass = bodyA->inverseMass() + bodyB->inverseMass();
    const float e = bodyA->restitution * bodyB->restitution; // combined restitution
    const Vector3f n = contact.normal;

    const Mat3x3 invWorldInertiaA = bodyA->getWorldInvInertiaTensor();
    const Mat3x3 invWorldInertiaB = bodyB->getWorldInvInertiaTensor();

    const Vector3f ra = ptOnA - bodyA->getCenterOfMassWorld();
    const Vector3f rb = ptOnB - bodyB->getCenterOfMassWorld();

    // Calculate the angular part of the impulse denominator
    const Vector3f va = bodyA->velocity + cross(bodyA->angularVelocity, ra);
    const Vector3f vb = bodyB->velocity + cross(bodyB->angularVelocity, rb);
    const Vector3f vab = va - vb;

    const Vector3f angularJA = cross(invWorldInertiaA * cross(ra, n), ra);
    const Vector3f angularJB = cross(invWorldInertiaB * cross(rb, n), rb);
    const float angularFactor = dot(angularJA + angularJB, n);

    const float impulseJ = (1.0f + e) * dot(vab, n) / (totalInverseMass + angularFactor);
    const Vector3f impulse = n * impulseJ;

    bodyA->applyImpulseAtPoint(-1.0 * impulse, ptOnA);
    bodyB->applyImpulseAtPoint( 1.0 * impulse, ptOnB);

    // friction part
    const float friction = bodyA->friction * bodyB->friction; // combined friction
    const Vector3f va2 = bodyA->velocity + cross(bodyA->angularVelocity, ra);
    const Vector3f vb2 = bodyB->velocity + cross(bodyB->angularVelocity, rb);
    const Vector3f vab2 = va2 - vb2;

    const Vector3f velNorm = n * dot(n, vab2);
    const Vector3f velTang = vab2 - velNorm;

    if(velTang.magSqrd() > 1e-10)
    {
        Vector3f relativeTangent = velTang.normalize();
        const Vector3f inertiaA = cross(invWorldInertiaA * cross(ra, relativeTangent), ra);
        const Vector3f inertiaB = cross(invWorldInertiaB * cross(rb, relativeTangent), rb);
        const float frictionDenominator = dot(inertiaA + inertiaB, relativeTangent);

        const float reducedMass = 1.0f / (totalInverseMass + frictionDenominator);
        const Vector3f impulseFric = velTang * (friction * -reducedMass);

        bodyA->applyImpulseAtPoint( 1.0f * impulseFric, ptOnA);
        bodyB->applyImpulseAtPoint(-1.0f * impulseFric, ptOnB);
    }

    // move objects out of each other

    const float ta = bodyA->inverseMass() / totalInverseMass;
    const float tb = bodyB->inverseMass() / totalInverseMass;

    const Vector3f ds = ptOnB - ptOnA;

    bodyA->translate( 1.0 * ds * ta);
    bodyB->translate(-1.0 * ds * tb);
}
