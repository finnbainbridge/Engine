#include "Engine/Element3D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "Engine/DevTools.hpp"

using namespace Engine::E3D;

Element3D::Element3D(std::shared_ptr<Document> parent_document): DOM::Element(parent_document),
transform(),
transform_lock()
{
    setTagName("element3d");
    float aaa[16] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };

    transform_lock.lock();
    transform = glm::make_mat4(aaa);
    transform_lock.unlock();

    global_transform_lock.lock();
    global_transform = glm::make_mat4(aaa);
    global_transform_lock.unlock();

    addDevToolsButton("Orbit", std::bind(&Element3D::devtoolsOrbit, this));
}

void Element3D::devtoolsOrbit()
{
    LOG_ASSERT_MESSAGE_FATAL(!document->devtools->is_active, "Devtools must be active for devtoolsOrbit to be called");
    document->devtools->targetCamera(std::dynamic_pointer_cast<Element3D>(shared_from_this()));
}

glm::mat4 Element3D::getTransform() const
{
    return transform;
}

glm::mat4 Element3D::getGlobalTransform() const
{
    return global_transform;
}

void Element3D::rotate(float angle, glm::vec3 axis)
{
    transform_lock.lock();
    transform = glm::rotate(transform, angle, axis);
    transform_lock.unlock();
    callChildUpdate();
}

void Element3D::rotateGlobal(float angle, glm::vec3 axis)
{
    transform_lock.lock();
    transform = glm::rotate(transform, angle, glm::vec3(glm::inverse(transform) * glm::vec4(axis, 0)));
    transform_lock.unlock();
}

void Element3D::translate(glm::vec3 offset)
{
    transform_lock.lock();
    transform = glm::translate(transform, offset);
    transform_lock.unlock();
    callChildUpdate();
}

void Element3D::scale(glm::vec3 scaler)
{
    transform_lock.lock();
    transform = glm::scale(transform, scaler);
    transform_lock.unlock();
    callChildUpdate();
}

void Element3D::updateGlobalTransform()
{
    // Find parent
    global_transform_lock.lock();
    auto parents = getParentsByTagName("element3d", true);
    if (parents.size() < 1)
    {
        global_transform_lock.unlock();
        // std::cout << tag_name << " could not get element3d parent" << std::endl;
        return;
    }

    std::shared_ptr<Element3D> parent = std::dynamic_pointer_cast<Element3D>(parents[0]);
    if (parent == nullptr)
    {
        // My local is my global
        // How did this even get called
        global_transform_lock.unlock();
        
    }
    else {
        // Recalculate the global transform
        global_transform = parent->getGlobalTransform() * parent->getTransform();
        // global_transform = parent->getTransform() * parent->getGlobalTransform();
        global_transform_lock.unlock();
    }
}

void Element3D::callChildUpdate()
{
    auto element3ds = getElementsByTagName("element3d", true);
    for (int i = 0; i < element3ds.size(); i++)
    {
        std::shared_ptr<Element3D> child = std::dynamic_pointer_cast<Element3D>(element3ds[i]);

        child->updateGlobalTransform();
    }
}

CameraElement3D::CameraElement3D(std::shared_ptr<Document> document): Element3D::Element3D(document)
{
    setTagName("camera3d");
}

glm::mat4 CameraElement3D::_getViewMatrix()
{
    return glm::inverse(global_transform * transform);
}

// ================================================
// ManualMeshElement
ManualMeshElement3D::ManualMeshElement3D(std::shared_ptr<Document> parent_document): Element3D::Element3D(parent_document)
{
    setTagName("manualmesh3d");
}

void ManualMeshElement3D::setMesh(std::vector<glm::vec3> points, std::vector<glm::vec3> normals, std::vector<glm::vec2> tex_coords, std::vector<glm::uint32> indicies, std::shared_ptr<Renderer::ShaderProgram> shaders)
{
    if (given_points == true)
    {
        render_object->destroy();
    }

    render_object = document->renderer->addRenderObject();
    render_object->setMeshData(points, points, tex_coords, indicies);
    render_object->setShaderProgram(shaders);
}

void ManualMeshElement3D::render(float delta)
{
    global_transform_lock.lock();
    transform_lock.lock();
    document->renderer->renderRenderObject(render_object, global_transform, transform);
    global_transform_lock.unlock();
    transform_lock.unlock();
}