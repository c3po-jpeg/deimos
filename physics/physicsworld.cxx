#include "headers/physicsworld.hxx"
#include "headers/collision.hxx"


void PhysicsWorld::addRigidBody(RigidBody rb)
{
    m_rigidBodies.push_back(std::make_unique<RigidBody>(rb));
}

void PhysicsWorld::intergrate(float dt)
{
    for (auto &body: m_rigidBodies) 
    {
        body->update(dt);
    }

}

void PhysicsWorld::handleCollisions()
{
    for (size_t i = 0; i < m_rigidBodies.size(); ++i)
    {
        for (size_t j = i + 1; j < m_rigidBodies.size(); ++j)
        {
            auto a = m_rigidBodies[i].get();
            auto b = m_rigidBodies[j].get();

            auto contact = testCollision(a, b);

            if(contact.hasCollision)
            {
                resolveCollision(contact);
            }
        }
    }
}