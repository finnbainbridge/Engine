#include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
// #include "Engine/Threading.hpp"
#include "Engine/DevTools.hpp"
#include <memory>


Engine::Document::Document()
:element_types()
{

#ifndef ENGINE_NO_THREADING
    operations = std::vector<std::thread*>();
#endif
}

Engine::Document::~Document()
{
    // TODO: Change this
    //base->destroy();
}

void Engine::Document::setup()
{
    // TODO: Also setup renderer
    // Create basic elements
    base = std::shared_ptr<DOM::Element>(new DOM::Element(shared_from_this()));
    base->setTagName("engine");

    // Create a head and a body
    head = std::shared_ptr<DOM::Element>(new DOM::Element(shared_from_this()));
    head->setTagName("head");
    base->appendChild(head);

    body = std::shared_ptr<DOM::Element>(new DOM::Element(shared_from_this()));
    body->setTagName("body");
    base->appendChild(body);

    devtools = std::shared_ptr<DevTools::DevTools>(new DevTools::DevTools(shared_from_this()));
    base->appendChild(devtools);

    auto devtoolstree = std::make_shared<Engine::DevTools::DevToolsTree>(shared_from_this());
    devtools->appendChild(devtoolstree);

    devtools->setVisible(false);
}

void Engine::Document::tick(float delta)
{
    executeElement(delta, base);
    renderElement(delta, base);

    Engine::Threading::waitForCompletion();
}

void Engine::Document::renderElement(float delta, std::shared_ptr<DOM::Element> element)
{
    if (element->inited == false)
    {
        element->init();
        element->inited = true;
    }

    if (element->getVisible() == false)
    {
        return;
    }

    element->render(delta);

    for (size_t i = 0; i < element->getChildren().size(); i++) {
        renderElement(delta, element->getChildren()[i]);
    }
    
}

void Engine::Document::executeElement(float delta, std::shared_ptr<DOM::Element> element)
{

    if (element->getProcess() == false)
    {
        return;
    }
    
    if (element->inited != false)
    {
        Engine::Threading::addTask(std::bind(&Engine::DOM::Element::process, element, delta));
    }
    //element->process(delta);
    //element->render(delta);

    for (size_t i = 0; i < element->getChildren().size(); i++)
    {
        executeElement(delta, element->getChildren()[i]);
    }
}

void Engine::Document::destroy()
{
    base->destroy();
    //self_ptr.reset();
}
