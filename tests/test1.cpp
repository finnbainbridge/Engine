#include <iostream>
#include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"

int main(int argc, char const *argv[])
{
    auto document = std::make_shared<Engine::Document>();
    document->setup();

    auto element_a = std::make_shared<Engine::DOM::Element>(document);
    auto element_b = std::make_shared<Engine::DOM::Element>(document);


    element_a->appendChild(element_b);
    return !element_a->hasChild(element_b);
}
