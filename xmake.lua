add_rules("mode.debug", "mode.release")
add_requires("libsdl", "glm", "glew")
includes("deps/libglw", "deps/regex_literals")
target("cut")
    set_kind("binary")
    add_files("*.cpp")
    add_files("*.rc")
    add_headerfiles("*.h", "*.hpp")
    add_packages("libsdl", "glm")
    add_deps("libglw", "regex_literals")
    set_languages("c++17")
    if is_mode("release") then
        add_defines("NDEBUG")
    else
        add_defines("DEBUG")
    end
    add_installfiles("res/(**)", {prefixdir = "bin/res"})
