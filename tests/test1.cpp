#include <iostream>
#include "Engine/Engine.hpp"
#include "Engine/DOM.hpp"

int main(int argc, char const *argv[])
{
    auto document = Engine::Document();

    auto element_a = document.createElement(new Engine::DOM::Element());
    auto element_b = document.createElement(new Engine::DOM::Element());

    element_a->appendChild(element_b);
    return !element_a->hasChild(element_b);
}
