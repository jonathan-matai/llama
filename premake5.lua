--[[
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > premake5.lua
 The project setup file for Premake5
]]

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

        defines { "_WIN32", "WINVER=0x0A00", "_WIN32_WINNT=0x0A00" }

    filter "system:linux"

        buildoptions { "-march=native" }

    filter "configurations:Debug"

        symbols "On"
        
        defines { "DEBUG" }

    filter "configurations:Release"

        optimize "On"

        defines { "NDEBUG" }

    group "vendor"

        include "Llama/vendor"

    group ""

    project "Llama"

        kind "SharedLib"
        location "Llama"

        pchheader "llpch.h"
        pchsource "%{prj.name}/source/llpch.cpp"

        includedirs
        {
            "%{prj.name}/include/llama",
            "%{prj.name}/vendor/asio/asio/include",
            "$(VULKAN_SDK)/Include",
            "%{prj.name}/vendor/glfw/include",
            "%{prj.name}/vendor/VulkanMemoryAllocators",
            "%{prj.name}/vendor/json",
            "%{prj.name}/vendor/stb"
        }

        files
        {
            "%{prj.name}/include/**.h",
            "%{prj.name}/include/**.inl",
            "%{prj.name}/source/**.cpp",
            "%{prj.name}/source/**.h",
            "%{prj.name}/vendor/json/json.cpp"
        }

        links
        {
            "glfw"
        }

        defines { "LLAMA_BUILD" }

        filter "files:Llama/source/llcore.cpp"

            flags { "NoPCH" }

            filter "files:Llama/vendor/**"

            flags { "NoPCH" }

        filter "system:windows"

            defines { "LLAMA_OS_WINDOWS", "LLAMA_OS=\"Windows\"", "WINVER=0x0A00", "_WIN32_WINNT=0x0A00", "_CRT_SECURE_NO_WARNINGS" }

        filter "system:linux"

            defines { "LLAMA_OS_LINUX", "LLAMA_OS=\"Linux\"" }

            buildoptions { "-march=native" }

            linkoptions { "-pthread", "-lstdc++fs", "-Wl,--no-as-needed", "-ldl" }

        filter "configurations:Debug"
        
            defines { "LLAMA_DEBUG", "LLAMA_CONFIG=\"Debug\"" }

        filter "configurations:Release"

            defines { "LLAMA_RELEASE", "LLAMA_CONFIG=\"Release\"" }

        


    project "Common_1"

        kind "StaticLib"
        location "Common_1"

        files
        {
            "%{prj.name}/include/**.h",
            "%{prj.name}/include/**.inl",
            "%{prj.name}/source/**.cpp",
            "%{prj.name}/source/**.h"
        }

        includedirs
        {
            "Llama/include"
        }

        links
        {
            "Llama"
        }

    project "Client_1"

        kind "ConsoleApp"
        location "Client_1"

        files
        {
            "%{prj.name}/source/**.cpp",
            "%{prj.name}/source/**.h"
        }

        includedirs
        {
            "Llama/include",
            "Common_1/include"
        }

        links
        {
            "Llama",
            "Common_1"
        }

    project "Server_1"

        kind "ConsoleApp"
        location "Server_1"

        files
        {
            "%{prj.name}/source/**.cpp",
            "%{prj.name}/source/**.h"
        }

        includedirs
        {
            "Llama/include",
            "Common_1/include"
        }

        links
        {
            "Llama",
            "Common_1"
        }