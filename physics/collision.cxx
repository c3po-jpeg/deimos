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
        c.normal = ab.unit();
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

void resolveCollision(Contact &contact)
{
    RigidBody *bodyA = contact.bodyA;
    RigidBody *bodyB = contact.bodyB;     

    const float totalInverseMass = bodyA->inverseMass() + bodyB->inverseMass();

    const Vector3f n = contact.normal;

    const Vector3f vab = bodyB->velocity - bodyA->velocity;
    const float impulseJ = -2.0 * dot(vab, n) / totalInverseMass;
    const Vector3f impulse = impulseJ * n;

    bodyA->applyLinearImpulse(-1.0*impulse);
    bodyB->applyLinearImpulse( 1.0*impulse);

    const float ta = bodyA->inverseMass() / totalInverseMass;
    const float tb = bodyB->inverseMass() / totalInverseMass;

    const Vector3f ds = contact.pointBWorldSpace - contact.pointAWorldSpace;

    bodyA->translate( 1.0 * ds * ta);
    bodyB->translate(-1.0 * ds * tb);
}