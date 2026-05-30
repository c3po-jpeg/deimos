#include "headers/cubesphere.hxx"

CubeSphere::CubeSphere(
        Core    &core,
        float    radius, 
        int      subdivisions, 
        Vector3f color) 
        : Shape(core), m_radius(radius), m_subdivisions(subdivisions)
{}

void CubeSphere::buildGeometry()
{

    // Per-face colours (used when m_color == {-1,-1,-1})
    std::array<Vector3f, 3> faceColors = {{
        {0.9f, 0.1f, 0.2f},  //  red
        {0.2f, 0.3f, 0.8f},  //  blue
        {0.9f, 0.8f, 0.1f},  //  yellow
    }};

    if (m_color.x > 0.0f)
    {
        for(auto &color: faceColors)
            color = m_color;
    }

    const float step = 2.0f / (float)m_subdivisions;
    //  ________
    // |__|__|__|  subdivide a square and project(normalize) points to a sphere
    // |__|__|__|  <- square with three divisions 
    // |__|__|__|
    // ___ params _____________________________________
    // start     : starting point. Has to be the top-left corner of each face because of the the way the uv is calculated
    // latitude  : a direction vector sweeping along the latitude of the surface 
    // longitude : a direction vector sweeping along the longitude of the surface
    // color     : self explanatory
    auto subDivideFace =  [&](Vector3f start, Vector3f latitude, Vector3f longitude, Color3f color) -> std::vector<Vertex3D>
    {
        std::vector<Vertex3D> vertices;

        for(int j = 0; j < (m_subdivisions + 1); ++j)
        { 
            for (int i = 0; i < (m_subdivisions + 1); ++i) 
            {                                              
                Vector3f position = start + ((float)i * latitude) + ((float)j * longitude);

                float u = (float)i / (float)(m_subdivisions + 1);
                float v = 1.0f -  ((float)j / float(m_subdivisions + 1));

                // Use cube-to-sphere projection: normalize, then adjust by sqrt(1 + x²/2 + y²/2)
                // This reduces area distortion at cube corners compared to simple normalization
                Vector3f normalized = position.unit();
                /* float factor = std::sqrt(1.0f + normalized.x * normalized.x * 0.5f + normalized.y * normalized.y * 0.5f);
                Vector3f adjusted = normalized * (1.0f / factor); */

                vertices.push_back(Vertex3D{
                    .pos    = normalized * m_radius,
                    .normal = normalized,
                    .uv     = {u, v},
                    .col    = color,
                });
            }                                        
            
        }

        return vertices;
    };

    
    std::vector<Vertex3D> faces[6] = {
        // +Z face
        subDivideFace({-1.0, 1.0, 1.0}, {step , 0.0, 0.0}, {0.0,-step, 0.0}, faceColors[0]),
        // -Z face
        subDivideFace({1.0, 1.0,-1.0},  {-step, 0.0, 0.0}, {0.0,-step, 0.0}, faceColors[0]),
        // +Y face
        subDivideFace({-1.0, 1.0,-1.0}, {step , 0.0, 0.0}, {0.0, 0.0, step}, faceColors[1]),
        // -Y face
        subDivideFace({1.0,-1.0,-1.0},  {-step, 0.0, 0.0}, {0.0, 0.0, step}, faceColors[1]),
        // +X face
        subDivideFace({1.0, 1.0, 1.0},  {0.0, 0.0,-step},  {0.0,-step, 0.0}, faceColors[2]),
        // -X face
        subDivideFace({-1.0, 1.0,-1.0}, {0.0, 0.0, step},  {0.0,-step, 0.0}, faceColors[2])
    
    };

    // index generation
    for (int i = 0; i < 6; i++)
    {
        size_t base = m_vertices.size();
        for (uint32_t row = 0; row < m_subdivisions; ++row)
        {
            for(uint32_t col = 0; col < m_subdivisions; ++col)
            {
                
                uint32_t idx1 = ((uint32_t)m_subdivisions + 1) * row        + col + 0;
                uint32_t idx2 = ((uint32_t)m_subdivisions + 1) * (row + 1)  + col + 0;
                uint32_t idx3 = ((uint32_t)m_subdivisions + 1) * row        + col + 1;

                uint32_t idx4 = ((uint32_t)m_subdivisions + 1) * row        + col + 1;
                uint32_t idx5 = ((uint32_t)m_subdivisions + 1) * (row + 1)  + col + 0;
                uint32_t idx6 = ((uint32_t)m_subdivisions + 1) * (row + 1)  + col + 1;

                m_indices.push_back(static_cast<uint32_t>(base) + idx1);
                m_indices.push_back(static_cast<uint32_t>(base) + idx2);
                m_indices.push_back(static_cast<uint32_t>(base) + idx3);

                m_indices.push_back(static_cast<uint32_t>(base) + idx4);
                m_indices.push_back(static_cast<uint32_t>(base) + idx5);
                m_indices.push_back(static_cast<uint32_t>(base) + idx6);
            }
            
        }

        auto face = faces[i];
        m_vertices.insert(m_vertices.end(), face.begin(), face.end());
    }
}
