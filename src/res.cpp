/*
====================
THIS FILE IS STUPID!
====================
*/
#include "Engine/Res.hpp"
#include <filesystem>
std::string directory = "";
std::map<std::string, std::shared_ptr<Engine::Res::IResource>> cache = std::map<std::string, std::shared_ptr<Engine::Res::IResource>>();

std::string Engine::Res::ResourceManager::dirname(std::string source)
{
    source.erase(std::find(source.rbegin(), source.rend(), '/').base(), source.end());
    return source;
}

void Engine::Res::ResourceManager::start(int argc, char const* argv[]) 
{
    std::string exe_path = argv[0];
    directory = std::filesystem::path(std::filesystem::path(exe_path).parent_path()).parent_path();
}

std::shared_ptr<Engine::Res::IResource> Engine::Res::ResourceManager::getCachedRes(std::string filename)
{
    std::shared_ptr<IResource> ptr;
    try {
        return cache.at(filename);
    }
    catch (std::exception sx)
    {
        return nullptr;
    }
}

void Engine::Res::ResourceManager::setCachedRes(std::string filename, std::shared_ptr<IResource> res)
{
    cache[filename] = res;
}

std::string Engine::Res::ResourceManager::getDirname()
{
    return directory;
}