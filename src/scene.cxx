#include "headers/scene.hxx"
#include "headers/camera.hxx"

#include <SDL2/SDL.h>

#include "../geometry/headers/shape.hxx"
#include "../geometry/headers/cube.hxx"
#include "../geometry/headers/cubesphere.hxx"
#include "../geometry/headers/icosphere.hxx"
#include "../geometry/headers/plane.hxx"
#include "../physics/headers/physicsworld.hxx"
#include "../physics/headers/rigidbody.hxx"
#include "../physics/headers/collider.hxx"
#include "../renderer/headers/core.hxx"
#include "../renderer/headers/drawable.hxx"

Scene::Scene(Core &core, int windowWidth, int windowHeight)
    : m_core(core)
{
    initializeCamera(windowWidth, windowHeight);
    initializePhysics();
    initializeLight();
}

Scene::~Scene() = default;

void Scene::initializeCamera(int windowWidth, int windowHeight)
{
    m_camera = std::make_unique<Camera>(
        Vector3f(0.0f, 0.0f, 3.0f),    // position: 3 units back
        -90.0f,                         // yaw: looking toward -Z
        0.0f,                           // pitch: level
        Vector3f(0.0f, 1.0f, 0.0f),     // world up
        45.0f,
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight));
}

void Scene::initializePhysics()
{
    m_physicsWorld = std::make_unique<PhysicsWorld>();
}

void Scene::initializeLight()
{
    m_light.direction = {0.6f, -1.0f, -0.4f, 0.0f};  // angled sun
    m_light.color     = {1.0f, 0.95f, 0.85f, 2.0f}; // warm white
    m_light.ambient   = {0.1f,  0.1f, 0.15f, 0.0f}; // cool ambient
}

void Scene::addCubeSphere(float x, float y, float z, float radius, int subDivisions,const Material &material, float mass, bool isStatic)
{
    // Physics body
    RigidBody sphereBody               = RigidBody(std::make_unique<SphereCollider>(radius));
    sphereBody.transform.translation   = Vector3f(x, y, z);
    if(isStatic){
        sphereBody.makeStatic();
    } else{
        /* sphereBody.restitution = 0.8f;
        sphereBody.friction    = 0.5f; */
        sphereBody.mass        = mass;
    }
    
    m_physicsWorld->addRigidBody(std::move(sphereBody));

    // Rendering sphere
    auto sphere = std::make_unique<CubeSphere>(m_core, radius, subDivisions);
    sphere->setMaterial(material);
    sphere->setPosition({x, y, z});
    sphere->upload();
    
    m_shapes.push_back(std::move(sphere));
    m_drawablesDirty = true;
}


void Scene::update(float deltaTime, float aspect)
{
    m_camera->setAspect(aspect);

    Vector3f camPos   = m_camera->getPosition();
    m_light.cameraPos = Vector4f(camPos.x, camPos.y, camPos.z, 1.0f);
    // Update physics
    m_physicsWorld->update(deltaTime);
    
    // Sync render state with physics bodies
    syncRenderables();
}

void Scene::handleInput(float deltaTime, const uint8_t *keys, int mouseX, int mouseY)
{
    if (keys)
    {
        if (keys[SDL_SCANCODE_W])
            m_camera->processKeyboard(CameraMovement::Forward, deltaTime);
        if (keys[SDL_SCANCODE_S])
            m_camera->processKeyboard(CameraMovement::Backward, deltaTime);
        if (keys[SDL_SCANCODE_A])
            m_camera->processKeyboard(CameraMovement::Left,     deltaTime);
        if (keys[SDL_SCANCODE_D])
            m_camera->processKeyboard(CameraMovement::Right,    deltaTime);
        if (keys[SDL_SCANCODE_E])
            m_camera->processKeyboard(CameraMovement::Up,       deltaTime);
        if (keys[SDL_SCANCODE_Q])
            m_camera->processKeyboard(CameraMovement::Down,     deltaTime);
    }

    if (mouseX != 0 || mouseY != 0)
    {
        m_camera->processMouse(static_cast<float>(mouseX), static_cast<float>(mouseY));
    }
}

void Scene::syncRenderables()
{
    // Sync physics bodies with rendering shapes
    // Match each physics body with its corresponding shape by index
    // Shapes are added in the same order as their corresponding physics bodies (after floor)
    // Sync dynamic bodies with shapes
    for (size_t bodyIdx = 0, shapeIdx = 0;
        bodyIdx < m_physicsWorld->m_rigidBodies.size() && shapeIdx < m_shapes.size(); 
        ++bodyIdx, ++shapeIdx)
    {
        const auto &body = m_physicsWorld->m_rigidBodies[bodyIdx];
        if (!body->isStatic())  // Skip static plane
        {
            m_shapes[shapeIdx]->setTransform(body->transform);
        }
        
    }

    m_drawablesDirty = true;
}

const std::vector<Drawable> &Scene::getDrawables() const
{
    if (m_drawablesDirty)
    {
        rebuildDrawables();
        m_drawablesDirty = false;
    }
    return m_cachedDrawables;
}

void Scene::rebuildDrawables() const
{
    m_cachedDrawables.clear();

    // Add all shapes in order
    for (const auto &shape : m_shapes)
    {
        m_cachedDrawables.push_back(shape->getDrawData());
    }
}
