#include "Engine/DevTools.hpp"
#include "Engine/Element3D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <math.h>
#include <memory>

using namespace Engine::DevTools;

const float aaa[16] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };

OrbitCamera3D::OrbitCamera3D(std::shared_ptr<Document> new_document)
: E3D::Element3D(new_document),
target(),
yaw(0),
pitch(0),
radius(10),
angle_x(0),
angle_y(0),
cam_lock(),
cam_pos()
{
    target = glm::vec3(0,0,0);
    setTagName("orbitcamera3d");
}

void OrbitCamera3D::init()
{
    // Thread lock it because Cameras have potencial to be inited twice at the same time
    cam_lock.lock();
    // Add the _actual_ camera
    camera = std::make_shared<E3D::CameraElement3D>(document);
    camera->translate(glm::vec3(0,0,10));
    appendChild(camera);

    cam_lock.unlock();
    // glm::vec4 x_axis = getGlobalTransform() * glm::vec4(1, 0, 0, 0);
    // glm::vec4 y_axis = getGlobalTransform() * glm::vec4(0, 1, 0, 0);
}

void OrbitCamera3D::process(float delta)
{
    // Check if the middle mouse button is pressed
    if (document->renderer->isMouseButtonPressed(ENGINE_MOUSE_BUTTON_3))
    {
        // Confine the mouse
        document->renderer->setMouseMode(Engine::Input::MouseMode::Confined);
        glm::vec2 offset = document->renderer->getMouseOffset();
        
        if (document->renderer->isKeyPressed(ENGINE_KEY_LEFT_SHIFT))
        {
            // Move the target
            translate(glm::vec3(offset.x*0.015, offset.y*-0.015, 0));
        }
        else
        {
            // Rotate the y globally, but the x locally
            rotate(offset.y*0.02f, glm::vec3(1, 0, 0));
            rotateGlobal(offset.x*0.02f, glm::vec3(0, 1, 0));
        }
    }
    else
    {
        document->renderer->setMouseMode(Engine::Input::MouseMode::Free);
    }

    // Deal with scroll wheel
    auto pos = camera->getTransform() * glm::vec4(0,0,0,1);
    float one = pos.z * 0.05;
    camera->translate(glm::vec3(0, 0, one * -document->renderer->getScrollWheelOffset()));
}

glm::mat4 OrbitCamera3D::_getViewMatrix()
{

    return camera->_getViewMatrix();
}

void OrbitCamera3D::setPosition(glm::vec3 pos)
{
    transform_lock.lock();
    transform = glm::make_mat4(aaa);
    transform_lock.unlock();
    translate(pos);
}