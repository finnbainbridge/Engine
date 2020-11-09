#include "Engine/Res.hpp"
#include "Engine/Log.hpp"
#include <cstring>
#include <filesystem>
#include <lz4.h>

glm::uint16 Engine::Res::ResourceManager::version = 1;

std::string directory = "";
std::map<std::string, std::shared_ptr<Engine::Res::IResource>> cache = std::map<std::string, std::shared_ptr<Engine::Res::IResource>>();

Engine::Res::FileType Engine::Res::IResource::file_type = FileType::text;

std::string Engine::Res::ResourceManager::dirname(std::string source)
{
    source.erase(std::find(source.rbegin(), source.rend(), '/').base(), source.end());
    return source;
}

void Engine::Res::ResourceManager::start(int argc, char const* argv[]) 
{
    // #ifdef __EMSCRIPTEN__
    std::string exe_path = argv[0];
    // TODO: Fix
#ifndef _MSC_VER
    std::string part_1 = (std::filesystem::path(exe_path).parent_path()).string();
    if (part_1 == "" || part_1 == "." || part_1 == "/")
    {
        directory = "..";
        return;
    }
    directory = std::filesystem::path(part_1).parent_path().string();
    if (directory == "" || directory == "/")
    {
        directory = ".";
    }
#else
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    directory = std::filesystem::path(path).parent_path().parent_path().parent_path().parent_path().string();
#endif
    // #else
    // std::string exe_path = std::filesystem::current_path();
    // directory = std::filesystem::path(exe_path).parent_path();
    // #endif
}

std::shared_ptr<Engine::Res::IResource> Engine::Res::ResourceManager::getCachedRes(std::string filename)
{
    // std::shared_ptr<IResource> ptr;
    try {
        return cache.at(filename);
    }
    catch (std::exception& sx)
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

char* Engine::Res::ResourceManager::compress(char* data, size_t size, int* out_size)
{
    auto dst_size = LZ4_compressBound(size);

    // One char = one byte. Remember that
    char* output = new char [dst_size];

    int out = LZ4_compress_default(data, output, size, dst_size);

    if (out == 0)
    {
        LOG_ERROR("LZ4 compression failed");
    }

    int size_var = out + sizeof(Lz4matHeader);
    std::memcpy(out_size, &size_var, sizeof(out));
    
    
    // Make the header
    Lz4matHeader header;
    header.version = version;
    header.size_compressed = out;
    header.size_uncompressed = size;

    // Allocate new array that's _actually the right size_
    // Also add the header
    char* real_output = new char [sizeof(Lz4matHeader) + out];

    // Copy in the header
    std::memcpy(real_output, &header, sizeof(Lz4matHeader));

    // Now copy in the relevant data
    std::memcpy(real_output + sizeof(Lz4matHeader), output, out);

    delete data;
    delete[] output;

    return real_output;
}

char* Engine::Res::ResourceManager::decompress(char* data, size_t size, int* out_size)
{
    // Load the header
    Lz4matHeader header;
    
    std::memcpy(&header, data, sizeof(Lz4matHeader));

    LOG_ASSERT_MESSAGE_FATAL(header.version != version, "File version is incorrect");
    LOG_ASSERT_MESSAGE_FATAL(header.size_compressed + sizeof(Lz4matHeader) != size, "File is malformed");

    char* output = new char [header.size_uncompressed];
    int out = LZ4_decompress_safe(data + sizeof(Lz4matHeader), output, header.size_compressed, header.size_uncompressed);

    if (out < 0)
    {
        LOG_ERROR("LZ4 decompression failed");
    }
    
    std::memcpy(out_size, &out, sizeof(out));

    delete data;
    
    return output;
}
