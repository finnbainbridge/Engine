#include <iostream>
#include <string>
#define ENGINE_NO_THREADING
#include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
// #include "Engine/Threading.hpp"
#include <memory>
#include <variant>

class TestElement : public Engine::DOM::Element
{
private:
    /* data */
public:
    TestElement(std::shared_ptr<Engine::Document> document);
    ~TestElement();
    virtual void process(float delta) override
    {

    };
protected:
};

TestElement::TestElement(std::shared_ptr<Engine::Document> document) : Element(document)
{

    setTagName("test");
}

TestElement::~TestElement()
{
}


int main(int argc, char const *argv[])
{
    Engine::Threading::startThreads();
    auto document = std::make_shared<Engine::Document>();
    document->setup();

    auto base = std::make_shared<Engine::DOM::Element>(document);
    int num = 50;

    for (size_t i = 0; i < num; i++)
    {
        auto lvl1_element = std::make_shared<Engine::DOM::Element>(document);
        base->appendChild(lvl1_element);

        for (size_t j = 0; j < num; j++)
        {
            auto lvl2_element = std::make_shared<Engine::DOM::Element>(document);
            lvl1_element->appendChild(lvl2_element);

            if (j < 5)
            {
                auto random_element = std::make_shared<TestElement>(document);
                lvl1_element->appendChild(random_element);
            }

            for (size_t k = 0; k < num; k++)
            {
                auto lvl3_element = std::make_shared<Engine::DOM::Element>(document);
                lvl2_element->appendChild(lvl3_element);

                if (k == 8)
                {
                    lvl2_element->setId("test");
                    lvl2_element->classList.add("hello");
                    lvl2_element->classList.add("world");
                    lvl2_element->classList.add("no");

                    lvl2_element->setAttribute("test", 2);
                }
            }

        }

    }

    document->body->appendChild(base);
    document->tick(1);
    document->destroy();
    Engine::Threading::cleanup();

    return 0;
}
