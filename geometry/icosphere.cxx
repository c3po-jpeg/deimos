#include "headers/icosphere.hxx"

Icosphere::Icosphere(Core &core, float radius, int subdivisions, Vector3f color)
    : Shape(core), m_radius(radius), m_subdivisions(subdivisions), m_color(color) {}

void Icosphere::buildGeometry()
{
    // Start with a regular icosahedron.
    // The golden ratio phi appears naturally in the coordinates of an icosahedron.
    const float v_angle  = std::atan(0.5f);  // ~26.565 degrees -- icosahedron geometry
    const float h_step   = 2.0f * PI / 5.0f; // 72 degrees between vertices

    // sin/cos of the vertical angle -- same for all belt vertices
    const float sinV = std::sin(v_angle);   //  0.4472 (y coordinate of upper belt)
    const float cosV = std::cos(v_angle);   //  0.8944 (xz radius of belt vertices)

    std::vector<Vertex3D>  vertices;
    std::vector<uint32_t>  indices;

    // Vertex counts per ring (6 each, not 5, for UV seam)
    // Ring indices:
    //   Ring 0 (top    poles):  0..5
    //   Ring 1 (upper  belt):   6..11
    //   Ring 2 (lower  belt):  12..17
    //   Ring 3 (bottom poles): 18..23

    // ---- Ring 0: top pole duplicates (y=1, all same 3D position) ----------
    // Each triangle that touches the top pole needs its own vertex with the
    // correct U coordinate for that triangle's centroid, otherwise all top
    // triangles share U=0 and the texture collapses to a point.
    for (int i = 0; i < 6; ++i)
    {
        float u = float(i) / 5.0f;
        vertices.push_back(Vertex3D{
            .pos    = {0.0f, m_radius, 0.0f},  // exactly at north pole
            .normal = {0.0f, 1.0f, 0.0f},
            .uv     = {u + 0.1f, 1.0f},         // offset by half-triangle width so
                                                // the U sits at the triangle centre
            .col    = m_color,
        });
    }

    // ---- Ring 1: upper belt (y = sin(v_angle)) -----------------------------
    for (int i = 0; i < 6; ++i)
    {
        float theta = i * h_step;
        float x     = std::cos(theta) * cosV;
        float z     = std::sin(theta) * cosV;
        float y     = sinV;

        // Normalize to unit sphere then scale by radius
        Vector3f p   = Vector3f(x, y, z); // already unit length for icosahedron
        Vector3f pos = p * m_radius;

        vertices.push_back(Vertex3D{
            .pos    = pos,
            .normal = p,                         // unit direction = correct normal on sphere
            .uv     = {float(i) / 5.0f, 0.75f},
            .col    = m_color,
        });
    }

    // ---- Ring 2: lower belt (y = -sin(v_angle)), rotated 36 degrees --------
    // The lower belt is offset by half a step (36 degrees) relative to the upper
    // belt -- this is the defining geometry of an icosahedron.
    for (int i = 0; i < 6; ++i)
    {
        float theta = (float(i) + 0.5f) * h_step; // 36-degree offset
        float x     = std::cos(theta) * cosV;
        float z     = std::sin(theta) * cosV;
        float y     = -sinV;

        Vector3f p   = Vector3f(x, y, z);
        Vector3f pos = p * m_radius;

        vertices.push_back(Vertex3D{
            .pos    = pos,
            .normal = p,
            .uv     = {(float(i) + 0.5f) / 5.0f, 0.25f},
            .col    = m_color,
        });
    }

    // ---- Ring 3: bottom pole duplicates (y=-1, all same 3D position) -------
    for (int i = 0; i < 6; ++i)
    {
        float u = (float(i) + 0.5f) / 5.0f;
        vertices.push_back(Vertex3D{
            .pos    = {0.0f, -m_radius, 0.0f},
            .normal = {0.0f, -1.0f, 0.0f},
            .uv     = {u, 0.0f},
            .col    = m_color,
        });
    }

    // =========================================================================
    // Indices -- 20 triangles total for a base icosahedron
    //
    //   5 top    triangles: ring0[i] -- ring1[i+1] -- ring1[i]
    //   10 middle triangles: 2 per column
    //   5 bottom triangles: ring2[i] -- ring2[i+1] -- ring3[i]
    // =========================================================================

    // Top cap: 5 triangles connecting pole to upper belt
    for (uint32_t i = 0; i < 5; ++i)
    {
        indices.push_back(i);           // ring 0 pole vertex i
        indices.push_back(6 + i + 1);  // ring 1 vertex i+1
        indices.push_back(6 + i);      // ring 1 vertex i
    }

    // Middle band: 10 triangles (2 per column)
    for (uint32_t i = 0; i < 5; ++i)
    {
        // Upper triangle of column (pointing down)
        indices.push_back(6  + i);      // ring1[i]
        indices.push_back(6  + i + 1);  // ring1[i+1]
        indices.push_back(12 + i);      // ring2[i]

        // Lower triangle of column (pointing up)
        indices.push_back(6  + i + 1);  // ring1[i+1]
        indices.push_back(12 + i + 1);  // ring2[i+1]
        indices.push_back(12 + i);      // ring2[i]
    }

    // Bottom cap: 5 triangles connecting lower belt to pole
    for (uint32_t i = 0; i < 5; ++i)
    {
        indices.push_back(12 + i);      // ring2[i]
        indices.push_back(12 + i + 1);  // ring2[i+1]
        indices.push_back(18 + i);      // ring3 pole vertex i
    }

    // =========================================================================
    // Subdivision
    //
    // Each triangle is split into 4 by inserting midpoints on each edge.
    // The midpoint is projected back onto the sphere (normalize then scale).
    //
    //        v1
    //       /  \
    //     m1----m3
    //     / \  / \
    //   v2---m2---v3
    //
    // UV midpoints are linearly interpolated -- this is an approximation but
    // works well for small triangles after a few subdivision levels.
    // The seam vertices (u=0 and u=1 at the same 3D position) cause some UV
    // discontinuity at the seam even after subdivision, which is a fundamental
    // limitation of cylindrical UV mapping on a closed surface.
    // =========================================================================

    auto midPoint = [&](const Vertex3D &a, const Vertex3D &b) -> Vertex3D
    {
        // Average positions then project onto sphere
        Vector3f mid    = ((a.pos + b.pos) * 0.5f).unit(); // unit direction
        Vector3f pos    = mid * m_radius;                   // scaled to radius

        // Linear UV interpolation
        Vector2f uv     = (a.uv + b.uv) * 0.5f;

        return Vertex3D{
            .pos    = pos,
            .normal = mid,  // unit direction is correct sphere normal
            .uv     = uv,
            .col    = m_color,
        };
    };

    for (int s = 0; s < m_subdivisions; ++s)
    {
        std::vector<Vertex3D>  newVerts;
        std::vector<uint32_t>  newIndices;
        newVerts.reserve  (vertices.size() * 4);
        newIndices.reserve(indices.size()  * 4);

        for (size_t j = 0; j < indices.size(); j += 3)
        {
            const Vertex3D &v1 = vertices[indices[j]];
            const Vertex3D &v2 = vertices[indices[j + 1]];
            const Vertex3D &v3 = vertices[indices[j + 2]];

            Vertex3D m1 = midPoint(v1, v2);
            Vertex3D m2 = midPoint(v2, v3);
            Vertex3D m3 = midPoint(v1, v3);

            // Each sub-triangle gets its own 3 vertices (no sharing).
            // Sharing would require a midpoint cache keyed on edge pairs --
            // for a physics engine this simpler approach is fine and avoids
            // the complexity of deduplication across UV seams.
            auto addTri = [&](const Vertex3D &a, const Vertex3D &b, const Vertex3D &c)
            {
                uint32_t base = static_cast<uint32_t>(newVerts.size());
                newVerts.push_back(a);
                newVerts.push_back(b);
                newVerts.push_back(c);
                newIndices.push_back(base);
                newIndices.push_back(base + 1);
                newIndices.push_back(base + 2);
            };

            addTri(v1, m1, m3);
            addTri(m1, v2, m2);
            addTri(m1, m2, m3);
            addTri(m3, m2, v3);
        }

        vertices = std::move(newVerts);
        indices  = std::move(newIndices);
    }

    m_vertices = std::move(vertices);
    m_indices.clear();
    m_indices.reserve(indices.size());
    for (auto idx : indices)
        m_indices.push_back(static_cast<uint32_t>(idx));

}
