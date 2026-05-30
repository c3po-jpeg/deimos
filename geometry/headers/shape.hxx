#ifndef SHAPE_HXX
#define SHAPE_HXX

#include <vector>
#include <array>

#include "../../renderer/headers/buffer.hxx"
#include "../../renderer/headers/vertex.hxx"
#include "../../renderer/headers/drawable.hxx"
#include "../../renderer/headers/material.hxx"
#include "../../renderer/headers/descriptors.hxx"

#include "../../math/headers/vec2.hxx"
#include "../../math/headers/vec3.hxx"
#include "../../math/headers/mat4.hxx"
#include "../../math/headers/transform.hxx"

class Core;

// =============================================================================
// Shape — non-owning base class for all renderable primitives
// =============================================================================
class Shape
{
public:
    explicit Shape(Core &core, Vector2f position = {0.0f, 0.0f});

    Shape(const Shape &)            = delete;
    Shape &operator=(const Shape &) = delete;

    // Movable so shapes can live in vectors
    Shape(Shape &&)            = default;
    Shape &operator=(Shape &&) = default;

    virtual ~Shape() = default;

    // Call once after construction to generate geometry and upload to the GPU.
    void upload();

    /**
     * Records all draw commands into @p cmd.
     * Called by Renderer::drawShape() — do not call directly.
     *
     * @param cmd    The active command buffer (inside a render pass).
     * @param layout The pipeline layout (needed for push constants).
     */
    //void draw(VkCommandBuffer cmd, VkPipelineLayout layout) const;

    Drawable getDrawData();

    // ---- Transform ----------------------------------------------------------
    void setPosition(Vector3f translation);
    void setRotation(Quat rotation);
    void setScale(Vector3f size);

    void setTransform(const Transform &t);

    // converted to column major automatically so don't call transpose() again
    Mat4x4 getModel() { return m_model.toMat4x4().transpose(); }

    // ---- Material ----------------------------------------------------------
    void setMaterial(const Material &mat)
    {
        m_material = mat;
        m_materialDescriptor.update(m_material.toUBO());
    }

    const Material &getMaterial() const { return m_material; }

    // Convenience
    void setColor(Vector3f color)
    {
        m_material.albedo = color;
        m_material.colorA = color;
    }

protected:
    Core                 &m_core;
    Buffer                m_vertexBuffer;
    Buffer                m_indexBuffer;
    Transform             m_model{};
    Material              m_material{};
    MaterialDescriptor    m_materialDescriptor;
    std::vector<Vertex3D> m_vertices;  // filled by buildGeometry()
    std::vector<uint32_t> m_indices;

    /**
     * Derived classes override this to populate m_vertices.
     * Called automatically by upload().
     */
    virtual void buildGeometry() = 0;
};

#endif