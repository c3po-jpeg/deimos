#include "headers/collision.hxx"
#include "headers/rigidbody.hxx"
#include "headers/collider.hxx"

Contact testSphereSphere(RigidBody *a, RigidBody *b)
{
    Contact c;

    float ra = dynamic_cast<SphereCollider *>(a->collider.get())->radius();
    float rb = dynamic_cast<SphereCollider *>(b->collider.get())->radius();
    float rTotal = ra + rb;

    // AB = OB - OA
    Vector3f ab = b->position - a->position;

    if (ab.magSqrd() <= (rTotal * rTotal))
    {
        Vector3f normal = ab.unit();
        c.hasCollision = true;
        c.normal = normal;
        c.pointAWorldSpace = a->position + normal * ra;
        c.pointBWorldSpace = b->position - normal * rb;
        c.bodyA = a;
        c.bodyB = b;
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

void resolveCollision(Contact &contact)
{
    RigidBody *a = contact.bodyA;
    RigidBody *b = contact.bodyB;

    Vector3f ptAWorld = contact.pointAWorldSpace;
    Vector3f ptBWorld = contact.pointBWorldSpace;

    float totalInvMass = a->invMass() + b->invMass();

    Vector3f normal = contact.normal;

    Vector3f ra = ptAWorld - a->centerOfMassWorld();
    Vector3f rb = ptBWorld - b->centerOfMassWorld();

    float restitution = a->restitution * b->restitution;

    Vector3f velA = a->linearVelocity + cross(a->angularVelocity, ra);
    Vector3f velB = b->linearVelocity + cross(b->angularVelocity, rb);
    Vector3f vab  = velA - velB;

    Vector3f angFactorA = cross(a->getWorldInvInertiaTesnsor() * cross(ra, normal), ra);
    Vector3f angFactorB = cross(b->getWorldInvInertiaTesnsor() * cross(rb, normal), rb);
    float angularFactor = dot(angFactorA + angFactorB, normal);

    float j = (1.0 + restitution) * dot(vab, normal) / (totalInvMass + angularFactor);
    Vector3f impulse = normal * j;

    a->applyImpulseAtPoint(-1.0 * impulse, ptAWorld);
    b->applyImpulseAtPoint( 1.0 * impulse, ptBWorld);

    // friction impulse

    float friction = a->friction * b->friction;

    Vector3f velA = a->linearVelocity + cross(a->angularVelocity, ra);
    Vector3f velB = b->linearVelocity + cross(b->angularVelocity, rb);
    Vector3f vab  = velA - velB;

    Vector3f velNormal  = normal * dot(normal, vab);
    Vector3f velTangent = vab - velNormal;
    float tangLenSqrd   = velTangent.magSqrd();

    if (tangLenSqrd > 1e-10f)
    {
        Vector3f tangDir = velTangent.unit();

        Vector3f angFricA    = cross(a->getWorldInvInertiaTesnsor() * cross(ra, tangDir), ra);
        Vector3f angFricB    = cross(b->getWorldInvInertiaTesnsor() * cross(rb, tangDir), rb);
        float invInertiaTang = dot(angFricA + angFricB, tangDir);

        float reducedMass    = 1.0f / (totalInvMass + invInertiaTang);
        Vector3f fricImpulse = velTangent * (-reducedMass * friction);

        a->applyImpulseAtPoint( 1.0 * fricImpulse, ptAWorld);
        b->applyImpulseAtPoint(-1.0 * fricImpulse, ptBWorld);
    }

    Vector3f ds = ptAWorld - ptBWorld;

    float ta = a->invMass() / totalInvMass;
    float tb = b->invMass() / totalInvMass;

    a->position += ds * ta;
    b->position -= ds * tb;
}