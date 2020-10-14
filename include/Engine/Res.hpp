#ifndef ENGINE_RES_H
#define ENGINE_RES_H

#include "Engine/Engine.hpp"
#include "Engine/Log.hpp"
#include "glm/fwd.hpp"
#include <algorithm>
// #include <bits/types/FILE.h>
#include <exception>
#include <fstream>
#include <ios>
#include <istream>
#include <string>
#include <memory>
#include <map>
#include <iostream>
#include <sstream>

namespace Engine {
    namespace Res {

        enum FileType { text, binary };

        class IResource {
            public:
                virtual void loadFile(std::shared_ptr<std::stringstream> data) {};
                static FileType file_type;

                /*
                "Current" filename of this resource. This is the filename it was last loaded from/saved to.
                If it hasn't been saved, nor loaded, this will be ""
                */
                std::string fname = "";

                virtual void saveFile(std::shared_ptr<std::stringstream> file)
                {

                }

                IResource() {};
        };

        // Class for storing ResourceManager's data, because storing it in Resource Manager caused a very weird error
        // class ResourcePit {
        //     public:
        //         static std::string directory;
        //         static std::map<std::string, std::shared_ptr<IResource>> cache;
        // };

        // std::string directory;
        // std::map<std::string, std::shared_ptr<IResource>> cache;
        struct Lz4matHeader
        {
            glm::uint16 version;
            glm::uint32 size_compressed;
            glm::uint32 size_uncompressed;
        };

        class ResourceManager {
            private:
                
            public:
                static glm::uint16 version;
                /*
                Compresses the given data with the LZ4 Block library. Takes a char* as input, which will be freed. Returns another char* containing the compressed data. 
                It also takes `size`, which is the size of `data`. Also needs a int* outsize, which will contain the size of the returned char*
                */
                static char* compress(char* data, size_t size, int* outsize);

                /*
                Decompresses the given data with the LZ4 Block library. Takes a char* as input, which will be freed. Returns another char* containing the uncompressed data. 
                It also takes `size`, which is the size of `data`. Also needs a int* outsize, which will contain the size of the returned char*
                */
                static char* decompress(char* data, size_t size, int* outsize);

                static std::string dirname(std::string source);

                static void start(int argc, char const* argv[]);

                static std::shared_ptr<IResource> getCachedRes(std::string filename);
                static void setCachedRes(std::string filename, std::shared_ptr<IResource> res);

                static std::string getDirname();

                // Loads a file into the given type. The type must be descended from IResource.
                // The filename should assume it's in the base directory of the project
                template<typename res_t>
                static std::shared_ptr<res_t> load(std::string filename, bool _decompress = false, FileType file_type = FileType::text, bool force_new = false)
                {
                    LOG_ASSERT_MESSAGE_FATAL(filename == "", "Filename must exist");
                    std::shared_ptr<res_t> ptr = std::dynamic_pointer_cast<res_t>(getCachedRes(filename));

                    if (force_new || ptr == nullptr)
                    {
                        LOG_INFO("Loading file: " + getDirname() + "/" + filename);
                        ptr = std::make_shared<res_t>();
                        
                        std::ifstream data(getDirname() + "/" + filename, std::ios::in|std::ios::binary|std::ios::ate);
                        if (!data.is_open())
                        {
                            LOG_ERROR("Failed to load file " + getDirname() + "/" + filename);
                            return nullptr;
                        }
                        // Load in all the data
                        std::streampos size;
                        char * memblock;

                        // Load all the data into a pointer
                        size = data.tellg();
                        memblock = new char [size];
                        data.seekg(0, std::ios::beg);
                        data.read(memblock, size);
                        data.close();

                        // Process, uncompress, etc
                        if (_decompress)
                        {
                            int out_size;
                            memblock = decompress(memblock, size, &out_size);
                            size = out_size / sizeof(char);
                        }

                        // Now put in a stringstream for the masses
                        std::shared_ptr<std::stringstream> ss = std::make_shared<std::stringstream>();
                        ss->write(memblock, size);
                        ss->seekg(0, std::ios::beg);

                        ((std::shared_ptr<IResource>)ptr)->loadFile(ss);

                        setCachedRes(filename, ptr);

                        delete memblock;
                    }

                    LOG_ASSERT_MESSAGE_FATAL(ptr == nullptr, "Resource loading failed both badly, and inexplicably");

                    ptr->fname = filename;
                    return ptr;
                }

                // Saves a resource to the given filename
                static void save(std::string filename, std::shared_ptr<IResource> resource, bool _compress = false, FileType file_type = FileType::text)
                {
                    LOG_ASSERT_MESSAGE_FATAL(filename == "", "Filename must exist");
                    std::shared_ptr<std::stringstream> ss = std::make_shared<std::stringstream>();
                    resource->saveFile(ss);

                    LOG_INFO("Saving file: " + getDirname() + "/" + filename);

                    std::ofstream file (getDirname() + "/" + filename, std::ios::out);
                    if (!file.is_open())
                    {
                        LOG_ERROR("Could not open file: " + filename);
                        return;
                    }
                    
                    // Load in all the data
                    std::streampos size;
                    char * memblock;

                    // Load all the data into a pointer
                    ss->seekg(0, std::ios::end);
                    size = ss->tellg();
                    memblock = new char [size];
                    ss->seekg(0, std::ios::beg);
                    ss->read(memblock, size);

                    // Process data and compress, etc
                    if (_compress)
                    {
                        int out_size;
                        memblock = compress(memblock, size, &out_size);
                        size = out_size / sizeof(char);
                    }

                    // Write it to the file
                    file.write(memblock, size);
                    file.close();

                    resource->fname = filename;

                    delete memblock;
                }
        };

        class TextResource: public IResource
        {
            private:
                std::string text;
            public:
                virtual void loadFile(std::shared_ptr<std::stringstream> data)
                {
                    text = data->str();
                };

                TextResource() {};

                std::string getText() const
                {
                    return text;
                }

                void setText(std::string txt)
                {
                    text = txt;
                }

                virtual void saveFile(std::shared_ptr<std::stringstream> file)
                {
                    // We are saving the text file as binary because we can't use "<<" on a pointer
                    // auto stream = ResourceManager::save(filename, FileType::binary);
                    // stream->write(getText().c_str(), sizeof(getText().c_str()));
                    // stream->close();
                    file->str(getText());
                }
        };
    }
}

#endif