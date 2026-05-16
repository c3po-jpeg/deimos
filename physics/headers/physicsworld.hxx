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
    ~PhysicsWorld() {}

    Vector3f m_gravity = Vector3f(0.0, -9.8, 0.0);
    std::vector<std::unique_ptr<RigidBody>> m_rigidBodies;

    void addRigidBody(RigidBody rb);

    void update(float dt) 
    {
        intergrate(dt);
        handleCollisions();
    }

private:
    void intergrate(float dt);
    void handleCollisions();

};

#endif