#include "Engine/Element3D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Log.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <stdexcept>
#include <string>

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

void Element3D::setTransform(glm::mat4 transfor)
{
    transform_lock.lock();
    transform = transfor;
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

glm::mat4 stringToMatrix(std::string s)
{
    std::string delimiter = " ";

    float aaa[16];
    int i = 0;

    try
    {
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            aaa[i] = std::stof(token);
            i += 1;
            s.erase(0, pos + delimiter.length());
        }
        aaa[i] = std::stof(s);
    }
    catch (std::invalid_argument)
    {
        LOG_ERROR("Could not load Matrix: Malformed string");
    }

    return glm::make_mat4(aaa);
}

// Saving and loading
void Element3D::onLoad()
{
    if (hasAttribute("transform"))
    {
        try
        {
            transform_lock.lock();
            transform = stringToMatrix(std::get<std::string>(getAttribute("transform")));
            transform_lock.unlock();
        }
        catch (std::bad_variant_access e)
        {
            LOG_ERROR("Loading file: Element3D's attribute transform was not in the correct format");
        }
    }
    else
    {
        LOG_WARN("Loading file: Element3D could not find transform attribute. Skipping");
    }

    // if (hasAttribute("global_transform"))
    // {
    //     try
    //     {
    //         global_transform_lock.lock();
    //         global_transform = stringToMatrix(std::get<std::string>(getAttribute("global_transform")));
    //         global_transform_lock.unlock();
    //     }
    //     catch (std::bad_variant_access e)
    //     {
    //         LOG_ERROR("Loading file: Element3D's attribute global_transform was not in the correct format");
    //     }
    // }
    // else
    // {
    //     LOG_WARN("Loading file: Element3D could not find global_transform attribute. Skipping");
    // }
}

// Saving
void Element3D::onSave()
{
    transform_lock.lock();
    setAttribute("transform", std::to_string(transform[0][0]) + " " + std::to_string(transform[0][1]) + " " + std::to_string(transform[0][2]) + " " + std::to_string(transform[0][3]) + " " + 
                    std::to_string(transform[1][0]) + " " + std::to_string(transform[1][1]) + " " + std::to_string(transform[1][2]) + " " + std::to_string(transform[1][3]) + " " +
                    std::to_string(transform[2][0]) + " " + std::to_string(transform[2][1]) + " " + std::to_string(transform[2][2]) + " " + std::to_string(transform[2][3]) + " " +
                    std::to_string(transform[3][0]) + " " + std::to_string(transform[3][1]) + " " + std::to_string(transform[3][2]) + " " + std::to_string(transform[3][3]));

    transform_lock.unlock();

    // global_transform_lock.lock();
    // setAttribute("global_transform", std::to_string(global_transform[0][0]) + " " + std::to_string(global_transform[0][1]) + " " + std::to_string(global_transform[0][2]) + " " + std::to_string(global_transform[0][3]) + " " + 
    //                 std::to_string(global_transform[1][0]) + " " + std::to_string(global_transform[1][1]) + " " + std::to_string(global_transform[1][2]) + " " + std::to_string(global_transform[1][3]) + " " +
    //                 std::to_string(global_transform[2][0]) + " " + std::to_string(global_transform[2][1]) + " " + std::to_string(global_transform[2][2]) + " " + std::to_string(global_transform[2][3]) + " " +
    //                 std::to_string(global_transform[3][0]) + " " + std::to_string(global_transform[3][1]) + " " + std::to_string(global_transform[3][2]) + " " + std::to_string(global_transform[3][3]));

    // global_transform_lock.unlock();
}

void Element3D::appendChild(std::shared_ptr<DOM::Element> elem)
{
    if (elem->type_container.isType(document->element_types.getTypeOfElement("element3d")))
    {
        callChildUpdate();
    }

    DOM::Element::appendChild(elem);
}

void Element3D::onParentAdded()
{
    // if (getParent()->type_container.isType(document->element_types.getTypeOfElement("element3d")))
    // {
        // callChildUpdate();
    // }
    // else
    // {
    callChildUpdate();
    // }
}

// ====================================================
// Camera

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
    render_object->setMeshDataManual(points, points, tex_coords, indicies);
    render_object->setShaderProgram(shaders);
}

void ManualMeshElement3D::render(float delta)
{
    global_transform_lock.lock();
    transform_lock.lock();
    // document->renderer->renderRenderObject(render_object, global_transform, transform);
    global_transform_lock.unlock();
    transform_lock.unlock();
}

// ==========================================================
// Extension
void E3DExtension::start(std::shared_ptr<Document> doc)
{
    doc->addElement("element3d", std::make_shared<DOM::ElementClassFactory<Element3D>>());
    doc->addElement("camera3d", std::make_shared<DOM::ElementClassFactory<CameraElement3D>>());
    doc->addElement("manualmesh3d", std::make_shared<DOM::ElementClassFactory<ManualMeshElement3D>>());
    doc->addElement("mesh3d", std::make_shared<DOM::ElementClassFactory<MeshElement3D>>());
    doc->addElement("light", std::make_shared<DOM::ElementClassFactory<LightElement3D>>());
}