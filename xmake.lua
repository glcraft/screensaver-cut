add_rules("mode.debug", "mode.release")
add_requires("libsdl", "glm", "glew")
includes("deps/libglw", "deps/regex_literals")
target("cut")
    set_kind("binary")
    add_files("src/*.cpp")
    if (is_plat("windows")) then
        add_files("src/*.rc")
    end
    add_includedirs("include")
    add_headerfiles("include/*.h", "include/*.hpp")
    add_packages("libsdl", "glm")
    add_deps("libglw", "regex_literals")
    set_languages("c++17")
    if is_mode("release") then
        add_defines("NDEBUG")
    else
        add_defines("DEBUG")
    end
    add_installfiles("res/(**)", {prefixdir = "bin/res"})
