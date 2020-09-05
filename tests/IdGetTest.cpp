#include <iostream>
#include "Engine/Engine.hpp"
#include "Engine/DOM.hpp"
#include <memory>
#include <variant>

class TestElement : public Engine::DOM::Element
{
private:
    /* data */
public:
    TestElement(/* args */);
    ~TestElement();
protected:
};

TestElement::TestElement()
{
    setTagName("test");
}

TestElement::~TestElement()
{
}


int main(int argc, char const *argv[])
{
    auto document = Engine::Document();

    auto base = document.createElement(new Engine::DOM::Element());
    int num = 100;

    for (size_t i = 0; i < num; i++)
    {
        auto lvl1_element = document.createElement(new Engine::DOM::Element());
        base->appendChild(lvl1_element);

        for (size_t j = 0; j < num; j++)
        {
            auto lvl2_element = document.createElement(new Engine::DOM::Element());
            lvl1_element->appendChild(lvl2_element);

            if (j > 5)
            {
                auto random_element = document.createElement(new TestElement());
                lvl1_element->appendChild(random_element);
            }

            for (size_t k = 0; k < num; k++)
            {
                auto lvl3_element = document.createElement(new Engine::DOM::Element());
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
    
    auto res = base->getElementById("test");
    if (res == nullptr)
    {
        return 1;
    }

    res->classList.remove("no");
    if (!res->classList.has("hello") || !res->classList.has("world") || res->classList.has("no"))
    {
        return 1;
    }

    if(std::get<int>(res->getAttribute("test")) != 2)
    {
        return 1;
    }

    auto v = base->getElementsByTagName("test");
    std::cout << v.size() << std::endl;
    if (v.size() != 9400)
    {
        return 1;
    }
    
    return 0;
}
