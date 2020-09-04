#include "Engine/DOM.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>

using namespace Engine::DOM;

Element::Element()
: children(),
attributes(),
id(""),
classList()
{
}

Element::~Element()
{
    
}

std::string Element::getTagName()
{
    return tag_name;
}

void Element::appendChild(std::shared_ptr<Element> child)
{
    if (child->hasParent())
    {
        // Make sure it's not the a child of 2 elements
        child->getParent()->removeChild(child);
    }

    child->setParent(self_ptr);
    children.push_back(child);

    // Plan: Have a "self" pointer that gets set when added to a parent
    // Document will have a "set element" function
}

void Element::removeChild(std::shared_ptr<Element> child)
{
    children.remove(child);
    child->setParent(nullptr);
}

bool Element::hasChild(std::shared_ptr<Element> child)
{
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::list<std::shared_ptr<Element>> Element::getChildren()
{
    return children;
}

void Element::setParent(std::shared_ptr<Element> new_parent)
{
    // Do not run removeChild here 
    parent = new_parent;
}

std::shared_ptr<Element> Element::getParent()
{
    return parent;
}

bool Element::hasParent()
{
    if (parent == nullptr)
    {
        return false;
    }
    else
    {
        return true;
    }
}