#ifndef ICOSPHERE_SHAPE_HXX
#define ICOSPHERE_SHAPE_HXX

#include "shape.hxx"

class Icosphere : public Shape
{
public:
    explicit Icosphere(
        Core    &core,
        float    radius       = 0.5f,
        int      subdivisions = 3,
        Vector3f color        = {0.8f, 0.1f, 0.2f});
private:
    float    m_radius;
    int      m_subdivisions;
    Vector3f m_color;
    void buildGeometry() override;
};


#endif