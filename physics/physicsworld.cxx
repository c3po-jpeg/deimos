#include "headers/physicsworld.hxx"
#include "headers/collision.hxx"

void PhysicsWorld::addRigidBody(RigidBody &&rb)
{
    m_rigidBodies.push_back(std::make_unique<RigidBody>(std::move(rb)));
}

void PhysicsWorld::integrate(float dt)
{
    for (auto &body : m_rigidBodies)
    {
        if (!body->isStatic())
        {
            body->applyLinearImpulse(m_gravity * body->mass * dt);
            body->translate(body->velocity * dt);
        }
    }
}

void PhysicsWorld::handleCollisions()
{
    // Naive O(n^2) collision detection
    for (size_t i = 0; i < m_rigidBodies.size(); ++i)
    {
        for (size_t j = i + 1; j < m_rigidBodies.size(); ++j)
        {
            if (m_rigidBodies[i]->isStatic() && m_rigidBodies[j]->isStatic())
                continue; // Skip static-static pairs

            Contact contact = testCollision(m_rigidBodies[i].get(), m_rigidBodies[j].get());
            if (contact.hasCollision)
            {
                resolveCollision(contact);
            }
        }
    }
}