set(LIB_NAME fmod)
if(LORD_PLATFORM_WINDOWS)
    add_library(${LIB_NAME} SHARED IMPORTED GLOBAL)
    target_include_directories(${LIB_NAME} INTERFACE WIN32/Include)
    if(CMAKE_CL_64)
        set_target_properties(${LIB_NAME} PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/WIN32/Lib/fmod64.dll"
            IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/WIN32/Lib/fmod64_vc.lib")
    else()
        set_target_properties(${LIB_NAME} PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}WIN32/Lib/fmod.dll"
            IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/WIN32/Lib/fmod_vc.lib")
    endif()
elseif(LORD_PLATFORM_ANDROID)
    add_library(${LIB_NAME} SHARED IMPORTED GLOBAL)
    target_include_directories(${LIB_NAME} INTERFACE Android/Include)
    set_target_properties(${LIB_NAME} PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/Android/Lib/${ANDROID_ABI}/libfmod.so")
elseif(LORD_PLATFORM_IOS)
    add_library(${LIB_NAME} SHARED IMPORTED GLOBAL)
    target_include_directories(${LIB_NAME} INTERFACE IOS/Include)
    set_target_properties(${LIB_NAME} PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/IOS/Lib/libfmod_iphonesos.a")
elseif(LORD_PLATFORM_MAC)
    add_library(${LIB_NAME} SHARED IMPORTED GLOBAL)
    target_include_directories(${LIB_NAME} INTERFACE IOS/Include)
    set_target_properties(${LIB_NAME} PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/IOS/Lib/libfmod_iphonesos.a")
else()
    add_library(${LIB_NAME} INTERFACE)
endif()