#include <iostream>
#include "Engine/Engine.hpp"
// #include "Engine/DOM.hpp"
#include <memory>
#include <variant>

class TestElement : public Engine::DOM::Element
{
private:
    /* data */
public:
    TestElement(std::shared_ptr<Engine::Document> document);
    ~TestElement();
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
    std::cout << "Got ";
    auto document = std::make_shared<Engine::Document>();
    std::cout << "here" << std::endl;
    document->setup();

    auto base = std::make_shared<Engine::DOM::Element>(document);
    int num = 100;

    for (size_t i = 0; i < num; i++)
    {
        auto lvl1_element = std::make_shared<Engine::DOM::Element>(document);
        base->appendChild(lvl1_element);

        for (size_t j = 0; j < num; j++)
        {
            auto lvl2_element = std::make_shared<Engine::DOM::Element>(document);
            lvl1_element->appendChild(lvl2_element);

            if (j > 5)
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

    if (!base->contains(res))
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
