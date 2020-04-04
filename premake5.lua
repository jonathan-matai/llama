workspace "Llama"

    config = "%{cfg.buildcfg}-%{cfg.system}"

    architecture "x64"

    language "C++"
    cppdialect "C++17"

    configurations { "Debug", "Release" }
    startproject "Client_1"

    targetdir ("%{wks.location}/binaries/" .. config)
    objdir("%{wks.location}/build/" .. config .. "/%{prj.name}")

    filter "system:windows"
    
        systemversion "latest"

        defines { "LLAMA_OS_WINDOWS", "LLAMA_OS=\"Windows\"" }

    filter "system:linux"

        defines { "LLAMA_OS_LINUX", "LLAMA_OS=\"Linux\"" }

    filter "configurations:Debug"

        symbols "On"
        
        defines { "LLAMA_DEBUG", "DEBUG" }

    filter "configurations:Release"

        optimize "On"

        defines { "LLAMA_RELEASE", "NDEBUG" }

    project "Llama"

        kind "SharedLib"
        location "Llama"

        pchheader "llcore.h"
        pchsource "%{prj.name}/source/llcore.cpp"

        includedirs
        {
            "%{prj.name}/include/llama"
        }

        files
        {
            "%{prj.name}/include/**.h",
            "%{prj.name}/source/**.cpp",
            "%{prj.name}/source/**.h"
        }

        defines { "LLAMA_BUILD" }

    project "Client_1"

        kind "ConsoleApp"
        location "Client_1"

        files
        {
            "%{prj.name}/source/**.cpp",
            "%{prj.name}/soruce/**.h"
        }

        includedirs
        {
            "Llama/include"
        }

        links
        {
            "Llama"
        }

    project "Server_1"

        kind "ConsoleApp"
        location "Server_1"

        files
        {
            "%{prj.name}/source/**.cpp",
            "%{prj.name}/soruce/**.h"
        }

        includedirs
        {
            "Llama/include"
        }

        links
        {
            "Llama"
        }