#include "headers/plane.hxx"

Plane::Plane(Core &core, float size, Vector3f color, float tileUV)
    : Shape(core), m_size(size), m_color(color), m_tileUV(tileUV)
{}

void Plane::buildGeometry()
{
    const float s = m_size;
    const float t = m_tileUV;
    const Vector3f normal = {0.0f, 1.0f, 0.0f}; // points up

    // 4 corners of a flat XZ quad, Y = 0
    m_vertices = {
        {{-s, 0.0f,  s}, normal, {0.0f, 0.0f}, m_color},
        {{-s, 0.0f, -s}, normal, {   t, 0.0f}, m_color},
        {{ s, 0.0f, -s}, normal, {   t,    t}, m_color},
        {{ s, 0.0f,  s}, normal, {0.0f,    t}, m_color},
    };
    m_indices = {0, 2, 1, 0, 3, 2};
}
