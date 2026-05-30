#ifndef CUBESPHERE_SHAPE_HXX
#define CUBESPHERE_SHAPE_HXX

#include "shape.hxx"

class CubeSphere : public Shape
{
public:
    explicit CubeSphere(
        Core    &core,
        float    radius       = 0.5f,
        int      subdivisions = 3,
        Vector3f color        = {-1.0f, -1.0f, -1.0f});
private:
    float    m_radius       = 1.0f;
    int      m_subdivisions = 3;
    Vector3f m_color        = {-1.0f, -1.0f, -1.0f };
    void buildGeometry() override;
};

#endif