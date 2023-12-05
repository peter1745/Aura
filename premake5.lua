project "Aura"
    kind "StaticLib"

	warnings "Extra"

    files {
        "Aura/Include/Aura/**.ixx",
    }

	filter { "files:**.ixx" }
        compileas "Module"
