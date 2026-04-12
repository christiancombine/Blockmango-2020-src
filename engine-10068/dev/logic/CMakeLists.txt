cmake_minimum_required(VERSION 2.8)

if(COMMAND cmake_policy)  
	cmake_policy(SET CMP0015 NEW)  
endif(COMMAND cmake_policy)  

IF (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "None")  
    MESSAGE(STATUS "CMAKE_BUILD_TYPE is Debug")  
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Release")  
    MESSAGE(STATUS "CMAKE_BUILD_TYPE is Release")
ENDIF()

SET(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ALLOW_LOSSE_LOOP_CONSTRUCTS ON)

project(Logic)

message(STATUS "#####################################################################")
message(STATUS "# GameLogic Build System")
message(STATUS "#####################################################################")
message(STATUS "Configuring build system...")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")

# CMake 2.8.2 has a bug that creates unusable Xcode projects when using ARCHS_STANDARD_32_BIT
# to specify both armv6 and armv7.
#if(CMAKE_VERSION VERSION_EQUAL 2.8.2)
#	message(FATAL_ERROR "CMake 2.8.2 cannot create compatible Xcode projects for iOS")
#endif()

option(LORD_BUILD_PLATFORM_ANDROID "Android platform." FALSE)
	
if(UNIX)
	if (APPLE)
	message(STATUS "Mac iOS platform.")
		option(LORD_BUILD_PLATFORM_IOS "Mac iOS platform." TRUE)
	elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	    option(LORD_BUILD_PLATFORM_LINUX "Linux platform." TRUE)
	else()
		option(LORD_BUILD_PLATFORM_ANDROID "Android platform." TRUE)
	endif()
endif()

option(LORD_STATIC "Static build" TRUE)
option(LORD_UNICODE "Using Unicode charset" FALSE)

if(LORD_BUILD_PLATFORM_IOS AND LORD_BUILD_PLATFORM_ANDROID)
	message(FATAL_ERROR "Can only build for one platform.")
endif()

if(WIN32)
	if(LORD_BUILD_PLATFORM_ANDROID)
	
		set(LORD_PLATFORM_ANDROID TRUE)
		set(LORD_STATIC TRUE CACHE BOOL "Forcing static build for android" FORCE)
		
		#notice the "recycling" of CMAKE_C_FLAGS
		#this is necessary to pick up android flags
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -fPIC")
		
		message(STATUS "Platform: Android")
	else()
	
		set(LORD_PLATFORM_WIN32 TRUE)
		message(STATUS "Platform: Windows")
	
		# compiler building setting
		# Flags used by the compiler during all build types.
		set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /MP -we4150 -we4172 -we4390 -we4715")
		# Flags used by the compiler during debug builds.
		set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")
		# Flags used by the compiler during release minsize builds.
		set(CMAKE_CXX_FLAGS_MINSIZEREL "/MD /O1 /Ob1 /D NDEBUG")
		# Flags used by the compiler during release builds (/MD /Ob1 /Oi /Ot /Oy /Gs will produce slightly less optimized but smaller files).
		set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG")
		# Flags used by the compiler during Release with Debug Info builds.
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /D NDEBUG")
		# Flags used by the compiler during all build types.
		set(CMAKE_C_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000")
		# Flags used by the compiler during debug builds.
		set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /MDd /Zi  /Ob0 /Od /RTC1")
		# Flags used by the compiler during release minsize builds.
		set(CMAKE_C_FLAGS_MINSIZEREL "/MD /O1 /Ob1 /D NDEBUG")
		# Flags used by the compiler during release builds (/MD /Ob1 /Oi /Ot /Oy /Gs will produce slightly less optimized but smaller files).
		set(CMAKE_C_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG")
		# Flags used by the compiler during Release with Debug Info builds.
		set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /D NDEBUG")
		
		if(MSVC)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
			
			# Enable intrinsics on MSVC in debug mode
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Oi")
			if (CMAKE_CL_64)
				# Visual Studio bails out on debug builds in 64bit mode unless
				# this flag is set...
				set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
				set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
			endif ()
			
		endif()
		
		# This seems to break Xcode projects so definitely don't enable on Apple builds
		set(CMAKE_USE_RELATIVE_PATHS true)
		set(CMAKE_SUPPRESS_REGENERATION true)
		
	endif()
elseif(APPLE)
	message(STATUS "Platform: Mac iOS")
		INCLUDE(CMakeForceCompiler)
		CMAKE_FORCE_C_COMPILER(clang GNU)
		CMAKE_FORCE_CXX_COMPILER(clang++ GNU)
		set(CMAKE_SIZEOF_VOID_P 4)
		set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
		set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 9.0)
        set(CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS "armv7 armv7s arm64")
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
		set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH -Release "NO")
		set(CMAKE_XCODE_ATTRIBUTE_WARNING_CFLAGS "-Wno-deprecated-register -Wno-conversion -Wno-sign-conversion -Wno-switch -Wno-reorder 
			-Wno-tautological-constant-out-of-range-compare -Wno-missing-braces -Wno-unused-private-field -Wno-format-security 
			-Wno-unused-const-variable -Wno-format-nonliteral
			-Wno-format
			-Wno-unused-variable
			-Wno-tautological-compare
			-Wno-delete-non-virtual-dtor
			-Wno-inconsistent-missing-override
			-Wno-bitwise-op-parentheses
			-Wno-overloaded-virtual
			-Wno-logical-op-parentheses
			-Wno-unused-value
			-Wno-deprecated-declarations
            -Wno-shift-count-negative
            -Wno-shift-count-overflow")

		if(LORD_BUILD_PLATFORM_IOS)
			set(LORD_PLATFORM_IOS TRUE)

			set(LORD_STATIC TRUE CACHE BOOL "Forcing static build for iPhone" FORCE)
			message(STATUS "Platform: Mac iOS")
		else()
			set(LORD_PLATFORM_OSX TRUE)
			message(STATUS "Platform: Mac OSX")
		endif()
		
		if((CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX) AND NOT MINGW)
			set(XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
			
			if(LORD_PLATFORM_IOS)
        		set(CMAKE_OSX_SYSROOT iphoneos CACHE STRING "" FORCE)
				set(CMAKE_OSX_DEPLOYMENT_TARGET "")
				set(CMAKE_EXE_LINKER_FLAGS "-framework Foundation -framework CoreGraphics -framework QuartzCore -framework UIKit")
				set(XCODE_ATTRIBUTE_SDKROOT iphoneos CACHE STRING "" FORCE)
				set(XCODE_ATTRIBUTE_GCC_UNROLL_LOOPS "YES")
				set(XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
				set(XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")
				set(XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "YES")
				set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
				set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.perfect.\${PRODUCT_NAME:rfc1034identifier}")
				add_definitions(-mno-thumb)
				set(CMAKE_OSX_ARCHITECTURES ${ARCHS_STANDARD_32_BIT})
				# armv7 only
				# set(CMAKE_OSX_ARCHITECTURES ${ARCHS_UNIVERSAL_IPHONE_OS})
				remove_definitions(-msse)
			else()
				set(XCODE_ATTRIBUTE_SDKROOT macosx CACHE STRING "" FORCE)
				set(CMAKE_OSX_SYSROOT macosx CACHE STRING "" FORCE)
				#set(CMAKE_OSX_DEPLOYMENT_TARGET 10.5)
				if (NOT CMAKE_OSX_ARCHITECTURES)
					set(CMAKE_OSX_ARCHITECTURES ${ARCHS_STANDARD_32_64_BIT})
				endif()
        		
        		set(XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN[arch=x86_64] "YES")
			endif()
			
		endif()
elseif(UNIX)
	if(LORD_BUILD_PLATFORM_ANDROID)
	
		set(LORD_PLATFORM_ANDROID TRUE)
		set(LORD_STATIC TRUE CACHE BOOL "Forcing static build for android" FORCE)

		set(CMAKE_OSX_SYSROOT "" CACHE STRING "" FORCE)
		set(CMAKE_OSX_ARCHITECTURES "" CACHE STRING "" FORCE)
		set(XCODE_ATTRIBUTE_SDKROOT "" CACHE STRING "" FORCE)
		
		#notice the "recycling" of CMAKE_C_FLAGS
		#this is necessary to pick up android flags
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -fPIC")
		
		message(STATUS "Platform: Android")
	elseif(LORD_BUILD_PLATFORM_LINUX)
	
		message(STATUS "Platform: Unix/Linux")
		set(LORD_PLATFORM_LINUX TRUE)
		set(LORD_STATIC TRUE CACHE BOOL "Forcing static build for linux" FORCE)
		
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -std=c++11 --no-warnings -D_FORCE_INLINES")
	    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -std=c++11 --no-warnings -D_FORCE_INLINES")
	    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O2 -std=c++11 -Wall -Wfatal-errors -Wextra -D_FORCE_INLINES")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O2 -std=c++11 -Wall -Wfatal-errors -Wextra -D_FORCE_INLINES")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 --no-warnings -D_FORCE_INLINES")

	endif()
else()
	message(STATUS "Platform: Unknown")
endif()

message(STATUS "Set Environment Variables...")

if(CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
#  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(LOGIC_ROOT_PATH "${PROJECT_SOURCE_DIR}")
set(PROJECT_SOURCE_DIR "${LOGIC_ROOT_PATH}/Src")
set(LOGIC_DEP_PATH "${LOGIC_ROOT_PATH}/dependencies")
set(LORD_BIN_PATH "../../../engine-core/dev/engine/Bin")

if(LORD_PLATFORM_IOS)
    set(LORD_SRC_PATH "../../../engine-core/dev/engine/Src")
else()
    set(LORD_SRC_PATH "../../../engine-core/dev/engine/Src")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LOGIC_ROOT_PATH}/Bin/${CMAKE_BUILD_TYPE}")
if(LORD_PLATFORM_IOS)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LOGIC_ROOT_PATH}/../client/Shells/IOS/GameClothLibs/${CMAKE_BUILD_TYPE}")
else()
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LOGIC_ROOT_PATH}/Bin/${CMAKE_BUILD_TYPE}")
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LOGIC_ROOT_PATH}/Bin/${CMAKE_BUILD_TYPE}")
set(LORD_DEP_PATH "../../../engine-core/dev/engine/Dependencies")

message(STATUS "\${CMAKE_BUILD_TYPE}: ${CMAKE_BUILD_TYPE}")
message(STATUS "\${PROJECT_SOURCE_DIR}: ${PROJECT_SOURCE_DIR}")
message(STATUS "\${PROJECT_BINARY_DIR}: ${PROJECT_BINARY_DIR}")
message(STATUS "\${LOGIC_ROOT_PATH}: ${LOGIC_ROOT_PATH}")
message(STATUS "\${LORD_SRC_PATH}: ${LORD_SRC_PATH}")
message(STATUS "\${LORD_DEP_PATH}: ${LORD_DEP_PATH}")
message(STATUS "\${LOGIC_DEP_PATH}: ${LOGIC_DEP_PATH}")

# Add sub directories
add_subdirectory("Src")

# include jni
if(LORD_PLATFORM_ANDROID)
	include_directories("jni")
endif()
