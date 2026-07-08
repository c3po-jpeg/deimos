#ifndef PHYSICSWORLD_HXX
#define PHYSICSWORLD_HXX

#include <vector>
#include <memory>
#include "../../math/headers/vec3.hxx"
#include "rigidbody.hxx"

class PhysicsWorld
{
public:
    PhysicsWorld() = default;

    PhysicsWorld(const PhysicsWorld &)            = delete;
    PhysicsWorld &operator=(const PhysicsWorld &) = delete;
    ~PhysicsWorld() = default;

    Vector3f m_gravity = Vector3f(0.0, -9.8, 0.0);
    std::vector<std::unique_ptr<RigidBody>> m_rigidBodies;

    void addRigidBody(RigidBody &&rb);

    void update(const float dt) const
    {
        integrate(dt);
        handleCollisions();
    }

private:
    void integrate(float dt) const;
    void handleCollisions() const;

};

#endif