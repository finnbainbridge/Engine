// #include "Engine/DOM.hpp"
#include "Engine/Engine.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>


using namespace Engine::DOM;

Element::Element(std::shared_ptr<Engine::Document> parent_document)
: children(),
// attributes(),
id(""),
document(parent_document),
classList()
{
    setTagName("element");
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

    child->setParent(shared_from_this());
    children.push_back(child);

    // Plan: Have a "self" pointer that gets set when added to a parent
    // Document will have a "set element" function
}

void Element::removeChild(std::shared_ptr<Element> child)
{
    //children.remove(child);
    for (size_t i = 0; i < children.size(); i++)
    {
        if (children[i] == child)
        {
            children.erase(children.begin() + i);
            break;
        }
    }
    
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

std::vector<std::shared_ptr<Element>> Element::getChildren()
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

std::string Element::getId()
{
    return id;
}

void Element::setId(std::string new_id)
{
    id = new_id;
}

void Element::destroy()
{
    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->destroy();
    }

    // This probably frees this element, but I'm not really sure
    // self_ptr.reset();
}

// ==============================================
// Selectors

std::shared_ptr<Element> Element::getElementById(std::string id)
{
    for (size_t i = 0; i < children.size(); i++)
    {
        if (children[i]->getId() == id)
        {
            return children[i];
        }
        else {
            auto res = children[i]->getElementById(id);
            if (res != nullptr)
            {
                return res;
            }
        }
    }

    return nullptr;
}

std::vector<std::shared_ptr<Element>> Element::getElementsByTagName(std::string tag)
{
    std::vector<std::shared_ptr<Element>> output;

    for (size_t i = 0; i < children.size(); i++)
    {
        if (children[i]->getTagName() == tag)
        {
            output.push_back(children[i]);
        }

        auto more = children[i]->getElementsByTagName(tag);
        if (more.size() > 0)
        {
            output.insert(output.end(), more.begin(), more.end());
        }
    }

    return output;
}

bool Element::contains(std::shared_ptr<Element> element)
{
    for (size_t i = 0; i < children.size(); i++)
    {
        if (children[i] == element)
        {
            return true;
        }
        
        if (children[i]->contains(element))
        {
            return true;
        }
    }

    return false;
}

// =======================================================
// Attributes

void Element::setAttribute(std::string attribute, AttrVariant value)
{
    attributes[attribute] = value;
}

AttrVariant Element::getAttribute(std::string attribute)
{
    return attributes.at(attribute);
}

bool Element::hasAttribute(std::string attribute)
{
    try
    {
        attributes.at(attribute);
        return true;
    }
    catch(const std::exception& e)
    {
        return false;
    }
    
}

// =======================================================
// ClassList

ClassList::ClassList()
: classes()
{

}

ClassList::~ClassList()
{

}

void ClassList::add(std::string element_class)
{
    if (!has(element_class))
    {
        classes.push_back(element_class);
    }
}

void ClassList::remove(std::string element_class)
{
    if (!has(element_class))
    {
        std::cerr << "Cannot remove class " << element_class << ": Element does not contain this class" << std::endl;
        return;
    }

    for (size_t i = 0; i < classes.size(); i++)
    {
        if (classes[i] == element_class)
        {
            classes.erase(classes.begin() + i);
            break;
        }
    }
}

bool ClassList::has(std::string element_class)
{
    for (size_t i = 0; i < classes.size(); i++)
    {
        if (classes[i] == element_class)
        {
            return true;
        }
    }

    return false;
}
