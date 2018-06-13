project "lxpr"
	language "C++"
	kind "ConsoleApp"
	targetdir (bin_path .. "/%{cfg.platform}/%{cfg.buildcfg}/")
	location  (build_path .. "/lxpr/")

	files
	{
		"include/**.h",
		"src/**.cpp",
		gen_code_path .. "/**.h",
		gen_code_path .. "/**.cpp",
	}

	includedirs
	{
		"include/",
		cpprelude_path .. "/include/",
		rgx_path .. "/include/",
		oct_path .. "/include/",
		gen_code_path
	}

	links
	{
		"cpprelude",
		"rgx",
		"oct"
	}

	if os.istarget("linux") then

		buildoptions {"-std=c++14", "-Wall", "-fno-rtti", "-fno-exceptions"}
		linkoptions {"-pthread"}

		filter "configurations:debug"
			linkoptions {"-rdynamic"}

	elseif os.istarget("windows") then
		
		if os.getversion().majorversion == 10.0 then
			systemversion(win10_sdk_version())
		end

		buildoptions {"/utf-8", "/std:c++14"}
	end

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "platforms:x86"
		architecture "x32"

	filter "platforms:x64"
		architecture "x64"