#include "headers/physicsworld.hxx"
#include "headers/collision.hxx"

void PhysicsWorld::addRigidBody(RigidBody &&rb)
{
    m_rigidBodies.push_back(std::make_unique<RigidBody>(std::move(rb)));
}

void PhysicsWorld::integrate(const float dt) const
{
    for (const auto &body : m_rigidBodies)
    {
        if (!body->isStatic())
        {
            body->applyLinearImpulse(m_gravity * body->mass * dt);
            body->update(dt);
        }
    }
}

void PhysicsWorld::handleCollisions() const
{
    for (size_t i = 0; i < m_rigidBodies.size(); ++i)
    {
        for (size_t j = i + 1; j < m_rigidBodies.size(); ++j)
        {
            if (m_rigidBodies[i]->isStatic() && m_rigidBodies[j]->isStatic())
                continue; // Skip static-static pairs

            if (Contact contact = testCollision(m_rigidBodies[i].get(), m_rigidBodies[j].get());
                contact.hasCollision)
            {
                resolveCollision(contact);
            }
        }
    }
}