#ifndef ENGINE_RES_H
#define ENGINE_RES_H

#include <algorithm>
#include <exception>
#include <fstream>
#include <string>
#include <memory>
#include <map>

namespace Engine {
    namespace Res {

        enum FileType { text, binary };

        class IResource {
            public:
                virtual void loadFile(std::shared_ptr<std::ifstream> data);
                static FileType file_type;
        };

        class ResourceManager {
            private:
                static std::string directory;
                static std::map<std::string, std::shared_ptr<IResource>> cache;

            public:
                static std::string dirname(std::string source)
                {
                    source.erase(std::find(source.rbegin(), source.rend(), '/').base(), source.end());
                    return source;
                }

                static void start(int argc, char const* argv[]) 
                {
                    std::string exe_path = argv[0];
                    directory = dirname(dirname(exe_path));
                }

                // Loads a file into the given type. The type must be descended from IResource.
                // The filename should assume it's in the base directory of the project
                template<typename res_t>
                std::shared_ptr<res_t> load(std::string filename, FileType file_type = FileType::text, bool force_new = false)
                {
                    std::shared_ptr<res_t> ptr;
                    try {
                        cache.at(filename);
                    }
                    catch (std::exception sx)
                    {
                        
                        ptr = std::make_shared<res_t>();
                        if (file_type == FileType::text)
                        {
                            std::shared_ptr<std::ifstream> data = std::make_shared<std::ifstream>(directory + "/" + filename);
                            ((IResource)ptr).loadFile(data);
                        }
                        else if (file_type == FileType::binary)
                        {
                            std::shared_ptr<std::ifstream> data = std::make_shared<std::ifstream>(directory + "/" + filename, std::ios::binary);
                            ((IResource)ptr).loadFile(data);
                        }
                    }

                    return ptr;
                }
        };
    }
}

#endif