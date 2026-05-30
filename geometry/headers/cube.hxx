#ifndef CUBE_SHAPE_HXX
#define CUBE_SHAPE_HXX

#include "shape.hxx"

class Cube : public Shape
{
public:
    explicit Cube(
        Core &core, float size = 0.5f,
        Vector3f color = {-1.0f, -1.0f, -1.0f});
private:
    float     m_size;
    Vector3f  m_color;
    void buildGeometry() override;
};


#endif