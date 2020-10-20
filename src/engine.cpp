#include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
// #include "Engine/Threading.hpp"
#include "Engine/DevTools.hpp"
#include "Engine/Log.hpp"
#include "Engine/Res.hpp"
#include <exception>
#include <memory>
#include <string>
#include <variant>

#include "tinyxml2.h"
using namespace tinyxml2;


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

    // Load built in extensions
    addExtension(std::make_shared<E3D::E3DExtension>());

}

void Engine::Document::addElement(std::string name, std::shared_ptr<DOM::ElementClass> type)
{
    element_classes[name] = type;
}

bool is_int(const std::string & s)
{

    return (s.find_first_not_of( "-0123456789" ) == std::string::npos);
}

bool is_float(const std::string & s)
{
    return (s.find_first_not_of( ".0123456789" ) == std::string::npos);
}

std::shared_ptr<Engine::DOM::Element> Engine::Document::xmlElementToElement(XMLElement* node)
{
    std::shared_ptr<DOM::Element> element;

    // Find out type
    if (element_classes.find(node->Name()) != element_classes.end())
    {
        // The element exists!
        element = element_classes[node->Name()]->getNewInstance(shared_from_this());
    }
    else
    {
        LOG_WARN("Could not find element " + std::string(node->Name()) + ". Using base Element instead");
        element = std::make_shared<DOM::Element>(shared_from_this());
        element->setTagName(node->Name());
    }

    // TODO: Attributes, ids, classes, etc

    // Attributes aka the strange bit
    for (const XMLAttribute* child = node->FirstAttribute(); child != nullptr; child = child->Next())
    {
        DOM::AttrVariant value = "";
        std::string temp_v = child->Value();

        // Int
        if (is_int(temp_v)) {
            value = std::stoi(temp_v);
        }

        // Float
        else if (is_float(temp_v)) {
            value = std::stof(temp_v);
        }

        // String
        else
        {
            value = std::string(child->Value());
        }

        element->setAttribute(child->Name(), value);
    }

    if (element->hasAttribute("id"))
    {
        try {
            DOM::AttrVariant attr = element->getAttribute("id");
            std::string id = std::get<std::string>(attr);
            element->setId(id);
        }
        catch (std::bad_variant_access&)
        {
            LOG_ERROR("id was not a string");
        }
    }

    if (element->hasAttribute("class"))
    {
        try {
            DOM::AttrVariant attr = element->getAttribute("class");
            std::string clas = std::get<std::string>(attr);
            
            std::string delimiter = " ";
            size_t pos = 0;
            std::string token;
            while ((pos = clas.find(delimiter)) != std::string::npos) {
                token = clas.substr(0, pos);
                element->classList.add(token);
                clas.erase(0, pos + delimiter.length());
            }
            element->classList.add(clas);
        }
        catch (std::bad_variant_access&)
        {
            LOG_ERROR("id was not a string");
        }
    }

    element->onLoad();

    if (!node->NoChildren())
    {
        for (XMLElement* child = node->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
        {
            element->appendChild(xmlElementToElement(child));
        }
    }

    return element;

}

std::shared_ptr<Engine::DOM::Element> Engine::Document::loadFromFile(std::string name)
{
    std::string text = Engine::Res::ResourceManager::load<Engine::Res::TextResource>(name)->getText();
    
    // Create the document & load the xml
    XMLDocument doc;
    doc.Parse(text.c_str());

    auto base = doc.FirstChildElement();
    if (base == nullptr)
    {
        LOG_WARN("During file load: Could not find any elements. Returning nullptr");
        return nullptr;
    }

    return xmlElementToElement(base);
    // doc should be freed automatically
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
