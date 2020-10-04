#ifndef ENGINE_RES_H
#define ENGINE_RES_H

#include "Engine/Engine.hpp"
#include <algorithm>
#include <exception>
#include <fstream>
#include <string>
#include <memory>
#include <map>
#include <iostream>

namespace Engine {
    namespace Res {

        enum FileType { text, binary };

        class IResource {
            public:
                virtual void loadFile(std::shared_ptr<std::ifstream> data) {};
                static FileType file_type;

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
                    // std::shared_ptr<res_t> ptr = std::dynamic_pointer_cast<std::shared_ptr<res_t>>(getCachedRes(filename));
                    // For now F*** cache

                    // if (force_new || ptr == nullptr)
                    // {
                        LOG_INFO("Loaded file: " + getDirname() + "/" + filename);
                        auto ptr = std::make_shared<res_t>();
                        if (file_type == FileType::text)
                        {
                            std::shared_ptr<std::ifstream> data = std::make_shared<std::ifstream>(getDirname() + "/" + filename);
                            ((std::shared_ptr<IResource>)ptr)->loadFile(data);
                        }
                        else if (file_type == FileType::binary)
                        {
                            std::shared_ptr<std::ifstream> data = std::make_shared<std::ifstream>(getDirname() + "/" + filename, std::ios::binary);
                            ((std::shared_ptr<IResource>)ptr)->loadFile(data);
                        }

                        // setCachedRes(filename, ptr);
                    // }

                    return ptr;
                }
        };
    }
}

#endif