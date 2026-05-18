#ifndef RIGIDBODY_HXX
#define RIGIDBODY_HXX

#include "../../math/headers/math.hxx"
#include "collider.hxx"
#include <memory>

#define MAX_ANG_VEL 30.0f

struct RigidBody
{
    Vector3f position        = {0.0f, 0.0f, 0.0f};
    Quat     orientation     = Quat();
    Vector3f linearVelocity  = {0.0f, 0.0f, 0.0f};
    Vector3f angularVelocity = {0.0f, 0.0f, 0.0f};

    // ---- Physical properties -----------------------------------------------
    float mass           = 1.0f;
    float restitution    = 0.4f;  // bounciness [0 = dead stop, 1 = perfect bounce]
    float friction       = 0.5f;  // used in tangential impulse

    float invMass() const {
        if (std::isinf(mass))
            return 0.0;

        return 1.0f / mass;
    }  

    //Mat3x3   inertiaTensor;
    Mat3x3   getInvInertiaTensor() const;

    // ---- Collision geometry -------------------------------------------------
    std::unique_ptr<Collider> collider = nullptr;


    /**
     * Make this body completely immovable.
     * Equivalent to infinite mass -- absorbs any impulse without moving.
     */
    void makeStatic();

    bool isStatic() const
    {
        return mass == INFINITY;
    }

    Vector3f centerOfMassWorld() const;
    Vector3f centerOfMass() const;

    void update(float dt);

    // Apply a force at a world-space point (generates both force and torque)
    void applyImpulseAtPoint(Vector3f impulse, Point3f worldPoint);

    // Apply an instantaneous velocity change (bypasses mass, used by solver)
    void applyLinearImpulse(Vector3f impulse);

    // Apply an angular impulse (used by solver)
    void applyAngularImpulse(Vector3f impulse);

    Mat3x3 getWorldInvInertiaTesnsor() const;
};

#endif