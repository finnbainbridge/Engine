#include "Engine/Engine.hpp"
#include "Engine/DOM.hpp"
#include <memory>

Engine::Document::Document()
{

}

Engine::Document::~Document()
{

}

std::shared_ptr<Engine::DOM::Element> Engine::Document::createElement(Engine::DOM::Element* element)
{
    auto ptr = std::shared_ptr<Engine::DOM::Element>(element);
    ptr->self_ptr = ptr;
    return ptr;
}