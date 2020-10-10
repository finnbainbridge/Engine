#ifndef ENGINE_RES_H
#define ENGINE_RES_H

#include "Engine/Engine.hpp"
#include "Engine/Log.hpp"
#include <algorithm>
#include <bits/types/FILE.h>
#include <exception>
#include <fstream>
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
                virtual void loadFile(std::shared_ptr<std::ifstream> data) {};
                static FileType file_type;

                virtual void saveFile(std::string filename)
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

        class ResourceManager {
            private:
            public:
                static std::string dirname(std::string source);

                static void start(int argc, char const* argv[]);

                static std::shared_ptr<IResource> getCachedRes(std::string filename);
                static void setCachedRes(std::string filename, std::shared_ptr<IResource> res);

                static std::string getDirname();

                // Loads a file into the given type. The type must be descended from IResource.
                // The filename should assume it's in the base directory of the project
                template<typename res_t>
                static std::shared_ptr<res_t> load(std::string filename, FileType file_type = FileType::text, bool force_new = false)
                {
                    std::shared_ptr<res_t> ptr = std::dynamic_pointer_cast<res_t>(getCachedRes(filename));

                    if (force_new || ptr == nullptr)
                    {
                        LOG_INFO("Loaded file: " + getDirname() + "/" + filename);
                        ptr = std::make_shared<res_t>();
                        if (res_t::file_type == FileType::text)
                        {
                            std::shared_ptr<std::ifstream> data = std::make_shared<std::ifstream>(getDirname() + "/" + filename);
                            ((std::shared_ptr<IResource>)ptr)->loadFile(data);
                            data->close();
                        }
                        else if (res_t::file_type == FileType::binary)
                        {
                            std::shared_ptr<std::ifstream> data = std::make_shared<std::ifstream>(getDirname() + "/" + filename, std::ios::binary);
                            ((std::shared_ptr<IResource>)ptr)->loadFile(data);
                            data->close();
                        }

                        setCachedRes(filename, ptr);
                    }

                    LOG_ASSERT_MESSAGE_FATAL(ptr == nullptr, "Resource loading failed both badly, and inexplicably");

                    return ptr;
                }

                // Returnes a stream that can be written to. It will _not_ be automatically closed
                static std::shared_ptr<std::ofstream> save(std::string filename,  FileType file_type = FileType::text)
                {
                    std::shared_ptr<std::ofstream> data;
                    if (file_type == FileType::text)
                    {
                        data = std::make_shared<std::ofstream>(getDirname() + "/" + filename);
                    }
                    else
                    {
                        data = std::make_shared<std::ofstream>(getDirname() + "/" + filename, std::ios::binary);
                    }
                    return data;
                }
        };

        class TextResource: public IResource
        {
            private:
                std::string text;
            public:
                virtual void loadFile(std::shared_ptr<std::ifstream> data)
                {
                    std::stringstream ss;
                    ss << data->rdbuf();
                    // data->close();

                    text = ss.str();
                };

                TextResource() {};

                std::string getText() const
                {
                    return text;
                }

                virtual void saveFile(std::string filename)
                {
                    // We are saving the text file as binary because we can't use "<<" on a pointer
                    auto stream = ResourceManager::save(filename, FileType::binary);
                    stream->write(getText().c_str(), sizeof(getText().c_str()));
                    stream->close();
                }
        };
    }
}

#endif