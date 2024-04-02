cmake_minimum_required (VERSION 3.25)

project ("UnicodeExperiments")

# Enable Hot Reload for MSVC compilers if supported.
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
 
add_library (DefaultCompilerSettings INTERFACE)

if (MSVC)
	target_compile_options(DefaultCompilerSettings INTERFACE "/utf-8")
	target_compile_definitions(DefaultCompilerSettings INTERFACE
		_ENABLE_EXTENDED_ALIGNED_STORAGE
	)
endif()

if(WIN32)
	target_compile_definitions(DefaultCompilerSettings INTERFACE
		_WIN32_WINNT=0x0A00
		NOMINMAX
		UNICODE
		_UNICODE
	)
endif()

add_executable (UTF8Everywhere)
target_sources (UTF8Everywhere 
	PRIVATE
	"utf8everywhere.cpp"
	"utils.h"
)

if(WIN32)
target_sources (UTF8Everywhere 
	PRIVATE
	"cp-utf8.manifest"
)
endif()

target_link_libraries(UTF8Everywhere PRIVATE DefaultCompilerSettings)
set_target_properties(UTF8Everywhere PROPERTIES CXX_STANDARD 23)

if(WIN32)
	add_executable (UTF8OnLegacyWindows)
	target_sources (UTF8OnLegacyWindows 
		PRIVATE
		"utf8legacywindows.cpp"
		"utils.h"
	)
	target_link_libraries(UTF8OnLegacyWindows PRIVATE DefaultCompilerSettings)
	set_target_properties(UTF8OnLegacyWindows PROPERTIES CXX_STANDARD 23)
endif()