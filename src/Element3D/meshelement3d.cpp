#include "Engine/Element3D.hpp"
#include "Engine/Log.hpp"
#include "Engine/Res.hpp"
#include <variant>

using namespace Engine::E3D;

MeshElement3D::MeshElement3D(std::shared_ptr<Document> doc): Element3D(doc)
{
    setTagName("mesh3d");
}

void MeshElement3D::init()
{
    auto shader = document->renderer->addShaderProgram(Engine::Res::ResourceManager::load<Engine::Renderer::ShaderResource>("shaders/basic.vert"),
                Engine::Res::ResourceManager::load<Engine::Renderer::ShaderResource>("shaders/basic.frag"));
    
    setShaders(shader);
}

void MeshElement3D::setShaders(std::shared_ptr<Renderer::ShaderProgram> shaders)
{
    if (has_data == true)
    {
        render_object->setShaderProgram(shaders);
    }

    this->shaders = shaders;
}

void MeshElement3D::setResource(std::shared_ptr<Models::MeshResource> res)
{
    if (has_data == true)
    {
        // Kill the old render object
        render_object->destroy();
    }

    resource = res;

    render_object = document->renderer->addRenderObject();
    render_object->setMeshData(resource->getVertices(), resource->getIndices());

    has_data = true;
}

void MeshElement3D::_setResourceDry(std::shared_ptr<Models::MeshResource> res)
{
    resource = res;
}

void MeshElement3D::render(float delta)
{
    global_transform_lock.lock();
    transform_lock.lock();
    document->renderer->renderRenderObject(render_object, global_transform, transform);
    global_transform_lock.unlock();
    transform_lock.unlock();
}

void MeshElement3D::onSave()
{
    Element3D::onSave();
    setAttribute("resource", resource->fname);
}

void MeshElement3D::onLoad()
{
    Element3D::onLoad();
    if (!hasAttribute("resource"))
    {
        return;
    }

    auto attr = getAttribute("resource");
    LOG_ASSERT_MESSAGE_FATAL(!std::get_if<std::string>(&attr), "Attribute property must be a string");

    setResource(Res::ResourceManager::load<Models::MeshResource>(std::get<std::string>(attr), true));
}