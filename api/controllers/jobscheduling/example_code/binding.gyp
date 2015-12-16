{
	"targets":[
	{
		"target_name":"addon",
		"sources":[ "hello.cc", "jsoncpp.cpp" ],
		"xcode_settings": {
				"GCC_ENABLE_CPP_RTTI": "YES",
				"GCC_ENABLE_CPP_EXCEPTIONS": "YES",
				"OTHER_CPLUSPLUSFLAGS":["-stdlib=libc++"],
				"OTHER_LDFLAGS":["-stdlib=libc++"],
				"CLANG_CXX_LANGUAGE_STANDARD":"c++11",
				"MACOSX_DEPLOYMENT_TARGET":"10.7"
		}
	}
	]
}
