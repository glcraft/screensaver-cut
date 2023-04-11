add_rules("mode.debug", "mode.release")
add_requires("libsdl", "glm", "glew")
includes("deps/libglw", "deps/regex_literals")
target("cut")
    set_kind("binary")
    add_files("src/*.cpp")
    if (is_plat("windows")) then
        add_files("src/windows/*.cpp")
        add_files("src/windows/*.rc")
        add_packages("libsdl", "glm")
    elseif (is_plat("macosx")) then
        add_files("src/macos/*.cpp")
        add_files("src/macos/*.swift")
    end
    add_includedirs("include")
    add_headerfiles("include/*.h", "include/*.hpp")
    
    add_deps("libglw", "regex_literals")
    set_languages("c++17")
    add_defines((is_mode("release") and "N" or "") .. "DEBUG")

    -- on_build_files()
    
    add_installfiles("res/(**)", {prefixdir = "bin/res"})
