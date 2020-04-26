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

        defines { "LLAMA_OS_WINDOWS", "LLAMA_OS=\"Windows\"", "WINVER=0x0A00", "_WIN32_WINNT=0x0A00", "_CRT_SECURE_NO_WARNINGS" }

    filter "system:linux"

        defines { "LLAMA_OS_LINUX", "LLAMA_OS=\"Linux\"" }

        buildoptions { "-march=native" }

        linkoptions { "-pthread", "-lstdc++fs" }

    filter "configurations:Debug"

        symbols "On"
        
        defines { "LLAMA_DEBUG", "DEBUG", "LLAMA_CONFIG=\"Debug\"" }

    filter "configurations:Release"

        optimize "On"

        defines { "LLAMA_RELEASE", "NDEBUG", "LLAMA_CONFIG=\"Release\"" }

    group "vendor"

        include "Llama/vendor"

    group ""

    project "Llama"

        kind "SharedLib"
        location "Llama"

        pchheader "llcore.h"
        pchsource "%{prj.name}/source/llcore.cpp"

        includedirs
        {
            "%{prj.name}/include/llama",
            "%{prj.name}/vendor/asio/asio/include",
            "$(VULKAN_SDK)/Include",
            "%{prj.name}/vendor/glfw/include",
            "%{prj.name}/vendor/VulkanMemoryAllocators"
        }

        files
        {
            "%{prj.name}/include/**.h",
            "%{prj.name}/include/**.inl",
            "%{prj.name}/source/**.cpp",
            "%{prj.name}/source/**.h"
        }

        links
        {
            "glfw"
        }

        defines { "LLAMA_BUILD" }

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
            --,
            --"stdc++fs"
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
            --,
            --"stdc++fs"
        }