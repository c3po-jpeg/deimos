#include "headers/cube.hxx"

void Cube::buildGeometry()
{
    const float s = m_size;

    // Per-face colors (used when m_color == {-1,-1,-1})
    const std::array<Vector3f, 6> faceColors = {{
        {0.9f, 0.2f, 0.3f}, // +X  red
        {0.2f, 0.3f, 0.8f}, // -X  blue
        {0.3f, 0.8f, 0.1f}, // +Y  green
        {0.9f, 0.8f, 0.1f}, // -Y  yellow
        {0.7f, 0.0f, 0.9f}, // +Z  orange
        {0.2f, 0.8f, 0.9f}, // -Z  purple
    }};

    const bool useUniform = (m_color.x >= 0.0f);

    // Each face: 4 unique vertices (shared edges would have different normals
    // on adjacent faces, so we can't share vertices across faces without
    // losing per-face normals for lighting).
    struct FaceDesc
    {
        Vector3f normal;
        // 4 corners in CCW winding order
        std::array<Vector3f, 4> positions;
        std::array<Vector2f, 4> uvs;
    };

    const std::array<FaceDesc, 6> faces = {{
        // +X face (right)
        {
            {1, 0, 0},
            {{{s, -s, s}, {s, -s, -s}, {s, s, -s}, {s, s, s}}},
            {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}},
        // -X face (left)
        {
            {-1, 0, 0},
            {{{-s, -s, -s}, {-s, -s, s}, {-s, s, s}, {-s, s, -s}}},
            {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}},
        // +Y face (up)
        {
            {0, 1, 0},
            {{{-s, s, s}, {s, s, s}, {s, s, -s}, {-s, s, -s}}},
            {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}},
        // -Y face (down)
        {
            {0, -1, 0},
            {{{s, -s, s}, {-s, -s, s}, {-s, -s, -s}, {s, -s, -s}}},
            {{{0, 1}, {0, 0}, {1, 0}, {1, 1}}}},
        // +Z face (front)  towards camera by default since we look down -Z
        {
            {0, 0, 1},
            {{{-s, -s, s}, {s, -s, s}, {s, s, s}, {-s, s, s}}},
            {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}},
        // -Z face (back)
        {
            {0, 0, -1},
            {{{s, -s, -s}, {-s, -s, -s}, {-s, s, -s}, {s, s, -s}}},
            {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}},
    }};

    m_vertices.clear();
    m_indices.clear();

    for (uint32_t f = 0; f < 6; ++f)
    {
        const auto &face = faces[f];
        const Vector3f c = useUniform ? m_color : faceColors[f];
        const uint32_t base = static_cast<uint32_t>(m_vertices.size());

        for (uint32_t v = 0; v < 4; ++v)
            m_vertices.push_back({face.positions[v], face.normal, face.uvs[v], c});

        // Two triangles per face (CCW winding): 0-1-2 and 0-2-3
        m_indices.insert(
            m_indices.end(),
            {static_cast<uint32_t>(base + 0),
             static_cast<uint32_t>(base + 1),
             static_cast<uint32_t>(base + 2),
             static_cast<uint32_t>(base + 0),
             static_cast<uint32_t>(base + 2),
             static_cast<uint32_t>(base + 3)});
    }
}

Cube::Cube(Core &core, float size, Vector3f color)
    : Shape(core), m_size(size), m_color(color) {}