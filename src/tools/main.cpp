#include <iostream>
#include "Engine/Engine.hpp"
#include "Engine/Res.hpp"
#include "Engine/Tools/AssimpImporter.hpp"

int main(int argc, char const *argv[]) 
{
    // Start Engine
    Engine::Res::ResourceManager::start(argc, argv);

    assimp_import("../../assets/backpack.glb");
    return 0;

    std::string command;
    // Get argv
    if (argc < 2)
    {
        // No command
        command = "help";
    
    }
    else
    {
        // Deal with "homophone" commands
        std::string cm(argv[1]);
        if (cm == "-h" || cm == "--help")
        {
            command = "help";
        }
        else
        {
            command = cm;
        }
    }

    if (command == "help")
    {
        std::cout << "Engine tools:" << std::endl;
        std::cout << "Commands: " << std::endl;
        std::cout << "\thelp - Show this message" << std::endl;
        std::cout << "\timport <filename> - Convert the given 3D model into Engine's format" << std::endl;
    }
    else if (command == "import")
    {
        if (argc < 3)
        {
            std::cout << "Needs input file" << std::endl;
        }
        else
        {
            assimp_import(std::string(argv[2]));
        }
    }
    else
    {
        std::cout << "Invalid command \"" + command +"\"" << std::endl;
    }
}