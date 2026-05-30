#include "headers/shape.hxx"

Shape::Shape(Core &core, Vector2f position)
    : m_core(core),
      m_vertexBuffer(core),
      m_indexBuffer(core) ,
      m_materialDescriptor(core)
{
    m_model = Transform();
    m_materialDescriptor.update(m_material.toUBO());
}

void Shape::upload()
{
    // Let the derived class fill m_vertices
    buildGeometry();

    assert(!m_vertices.empty() && "Shape::upload — buildGeometry() left m_vertices empty!");

    m_vertexBuffer.create(
        sizeof(Vertex3D) * m_vertices.size(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_vertexBuffer.uploadDeviceLocal(m_vertices);

    if(!m_indices.empty())
    {
        m_indexBuffer.create(
            sizeof(uint32_t) * m_indices.size(),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_indexBuffer.uploadDeviceLocal(m_indices);
    }
}

Drawable Shape::getDrawData()
{
    return Drawable{
        .vertexBuffer     = m_vertexBuffer.get(),
        .indexBuffer      = m_indexBuffer.get(),   // VK_NULL_HANDLE if none
        .vertexCount      = static_cast<uint32_t>(m_vertices.size()),
        .indexCount       = static_cast<uint32_t>(m_indices.size()),
        .model            = getModel(),
        .material         = m_materialDescriptor,
    };
}

void Shape::setPosition(Vector3f translation)
{
    m_model.translation = translation;
}

void Shape::setRotation(Quat rotation)
{
    m_model.orientation = rotation;
}

void Shape::setScale(Vector3f s)
{
    m_model.scaling = s;
}

void Shape::setTransform(const Transform &t)
{
    m_model = t;
}
