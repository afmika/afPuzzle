-- premake5.lua
workspace "afPuzzle"
   configurations { "Debug", "Release" }

project "afPuzzle"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++14"

    targetdir "%{wks.location}/bin/%{cfg.buildcfg}/%{prj.name}"
    objdir "%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}"

    files {
        "src/**.h", 
        "src/**.cpp",
        "shaders/**"
    }

    filter { "system:windows" }
        includedirs "vendor/sfml/include"
        libdirs "vendor/sfml/lib"
        -- buildoptions { "-mwindows" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        links {
            "sfml-graphics-d",
            "sfml-window-d",
            "sfml-system-d",
            "sfml-audio-d"
        }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        links {
            "sfml-graphics",
            "sfml-window",
            "sfml-system",
            "sfml-audio"
        }