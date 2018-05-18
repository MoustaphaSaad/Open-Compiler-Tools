project "rgx"
	language "C++"
	kind "SharedLib"
	targetdir (bin_path .. "/%{cfg.platform}/%{cfg.buildcfg}/")
	location  (build_path .. "/rgx/")

	files
	{
		"include/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"include/",
		cpprelude_path .. "/include/"
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

		filter "configurations:debug"
			links {"dbghelp"}
	end

	filter "configurations:debug"
		targetsuffix "d"
		defines {"DEBUG", "RGX_DLL"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG", "RGX_DLL"}
		optimize "On"

	filter "platforms:x86"
		architecture "x32"

	filter "platforms:x64"
		architecture "x64"
