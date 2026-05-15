#ifndef RIGIDBODY_HXX
#define RIGIDBODY_HXX

#include "../../math/headers/math.hxx"
#include "collider.hxx"
#include <memory>

struct RigidBody
{
    Vector3f position        = {0.0f, 0.0f, 0.0f};
    Quat     orientation     = Quat();
    Vector3f linearVelocity  = {0.0f, 0.0f, 0.0f};
    Vector3f angularVelocity = {0.0f, 0.0f, 0.0f};

    // ---- Physical properties -----------------------------------------------
    float mass           = 1.0f;
    float invMass        = 1.0f;  // precomputed; 0 for static bodies
    float restitution    = 0.4f;  // bounciness [0 = dead stop, 1 = perfect bounce]
    float friction       = 0.5f;  // used in tangential impulse

    // ---- Inertia (diagonal of the inertia tensor in local space) -----------
    // Stored as inverse so division becomes multiplication in the solver.
    // Computed automatically by setMass() based on collider type.
    Mat3x3   inertiaTensor;
    Mat3x3   invInertiaTensor;

    // ---- Accumulated forces (cleared at the end of each step) --------------
    Vector3f forceAccum  = {0.0f, 0.0f, 0.0f};
    Vector3f torqueAccum = {0.0f, 0.0f, 0.0f};

    // ---- Collision geometry -------------------------------------------------
    std::unique_ptr<Collider> collider;

    void setMass(float m);

    /**
     * Make this body completely immovable.
     * Equivalent to infinite mass -- absorbs any impulse without moving.
     */
    void makeStatic();


    // Apply a force at a world-space point (generates both force and torque)
    void applyForceAtPoint(Vector3f force, Vector3f worldPoint);

    // Apply an instantaneous velocity change (bypasses mass, used by solver)
    void applyLinearImpulse(Vector3f impulse);

    // Apply an angular impulse (used by solver)
    void applyAngularImpulse(Vector3f impulse);

    Transform getTransform() const;

    Mat3x3 getWorldInvInertia() const;

    Vector3f velocityAtPoint(Vector3f worldPoint) const;


};

#endif