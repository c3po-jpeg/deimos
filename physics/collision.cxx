#include "headers/collision.hxx"
#include "headers/rigidbody.hxx"

Contact testSphereSphere(RigidBody &a, RigidBody &b) {
    Contact c;

    float ra = dynamic_cast<SphereCollider*>(a.collider.get())->radius();
    float rb = dynamic_cast<SphereCollider*>(b.collider.get())->radius();
    float rTotal = ra + rb;

    // AB = OB - OA
    Vector3f ab = b.position - a.position; 

    if(ab.magSqrd() <= (rTotal * rTotal)) 
    {
        Vector3f normal    = ab.unit();
        c.hasCollision     = true;
        c.normal           = normal;
        c.pointAWorldSpace = a.position + normal * ra;
        c.pointBWorldSpace = b.position - normal * rb;
        c.bodyA            = &a;
        c.bodyB            = &b;
    }

    return c;
}