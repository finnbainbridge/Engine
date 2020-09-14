#include <iostream>
#include <string>

#include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
// #include "Engine/Threading.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Amber.hpp"
#include "Engine/Res.hpp"
#include <memory>

class TriangleElement3D : public Engine::DOM::Element {
    private:
        std::shared_ptr<Engine::Renderer::RenderObject> model;
    public:
        TriangleElement3D(std::shared_ptr<Engine::Document> document);
        ~TriangleElement3D();
        virtual void render(float delta);
        void init();

    protected:
};

TriangleElement3D::TriangleElement3D(std::shared_ptr<Engine::Document> document)
    : Element(document) {

    setTagName("triangle");
}

void TriangleElement3D::init()
{
    auto shader = document->renderer->addShaderProgram(Engine::Res::ResourceManager::load<Engine::Renderer::ShaderResource>("shaders/basic.vert"),
                Engine::Res::ResourceManager::load<Engine::Renderer::ShaderResource>("shaders/basic.frag"));

    model = document->renderer->addRenderObject();
    model->setMeshData(std::vector<glm::vec3> {glm::vec3(0, 1, 0), glm::vec3(1, -1, 0), glm::vec3(-1, -1, 0)}, 
            std::vector<glm::vec3> {glm::vec3(0, -1, 0), glm::vec3(1, 1, 0), glm::vec3(-1, 1, 0)},
            std::vector<glm::vec2> {glm::vec2(0, -1), glm::vec2(1, 1), glm::vec2(-1, 1)},
            std::vector<glm::uint32> {0, 1, 2});
    model->setShaderProgram(shader);
}

TriangleElement3D::~TriangleElement3D() {}

void TriangleElement3D::render(float delta)
{
    model->draw();
}

int main(int argc, char const* argv[])
{
    Engine::Res::ResourceManager::start(argc, argv);
    auto document = Engine::Document::createDocument();
    auto renderer = std::make_shared<Engine::Renderer::Amber>(document);
    renderer->createWindow(1024, 866, "Test");

    auto tri_element = std::make_shared<TriangleElement3D>(document);
    document->body->appendChild(tri_element);
    tri_element->init();

    renderer->mainloop();
}