#include <iostream>
#include <string>

#include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
// #include "Engine/Threading.hpp"
#include "Engine/Renderer/Amber.hpp"
#include <memory>

int main(int argc, char const* argv[])
{
    auto document = Engine::Document::createDocument();
    auto renderer = std::make_shared<Engine::Renderer::Amber>(document);
    renderer->createWindow(1024, 866, "Test");

    renderer->mainloop();
}