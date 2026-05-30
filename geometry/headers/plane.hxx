#ifndef PLANE_SHAPE_HXX
#define PLANE_SHAPE_HXX

#include "shape.hxx"

class Plane : public Shape
{
public:
    /**
     * @param core   Core reference.
     * @param size   Half-extent in X and Z (total side = size * 2).
     * @param color  Surface colour.
     * @param tileUV UV tile count across the plane (useful for grid textures).
     */
    explicit Plane(
        Core &core,
        float    size   = 10.0f,
        Vector3f color  = {0.4f, 0.45f, 0.4f},
        float    tileUV = 1.0f);
private:
    float    m_size;
    Vector3f m_color;
    float    m_tileUV;
    void buildGeometry() override;
};

#endif