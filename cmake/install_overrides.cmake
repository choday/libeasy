#cmake .. -G "NMake Makefiles" -DCMAKE_USER_MAKE_RULES_OVERRIDE=E:\soft\prebuild\vs_install_overrides.cmake
#Â·¾¶¹æÔò
# -- set THIRD_PARTY_INCLUDE_DIR to E:/soft/prebuild/vs2019/include;E:/soft/prebuild/include
# -- set THIRD_PARTY_LIBRARY_DIR to E:/soft/prebuild/vs2019/vs2019-x64-Debug-MD;E:/soft/prebuild/vs2019-x64-Debug-MD;E:/soft/prebuild/lib

if(MSVC)     
  # Use the static C library for all build types

  option( MSVC_LINK_STATIC_RUNTIME_LIBRARY "link to static  C and C++ runtime library(/MT /MTd)" OFF)

  if(MSVC_LINK_STATIC_RUNTIME_LIBRARY)
      foreach(var 
        CMAKE_C_FLAGS_DEBUG_INIT 
        CMAKE_C_FLAGS_RELEASE_INIT
        CMAKE_C_FLAGS_MINSIZEREL_INIT 
        CMAKE_C_FLAGS_RELWITHDEBINFO_INIT
        CMAKE_CXX_FLAGS_DEBUG_INIT 
        CMAKE_CXX_FLAGS_RELEASE_INIT
        CMAKE_CXX_FLAGS_MINSIZEREL_INIT 
        CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        )
            if(${var} MATCHES "/MD")
              string(REGEX REPLACE "/MD" "/MT" ${var} "${${var}}")
              #MESSAGE(STATUS  "${var}:${${var}}")
            endif()
       endforeach()
       
       MESSAGE(STATUS "link to static  C and C++ runtime library(/MT)")
       set(MSVC_MT_NAME "MT")
    else()
       MESSAGE(STATUS "link to static  C and C++ runtime library(/MD)")
       set(MSVC_MT_NAME "MD")
  endif(MSVC_LINK_STATIC_RUNTIME_LIBRARY)


  if(MSVC_VERSION EQUAL 1400)
    set(MSVC_NAME vs2005)
  elseif(MSVC_VERSION EQUAL 1500)
    set(MSVC_NAME vs2008)
  elseif(MSVC_VERSION EQUAL 1600)
    set(MSVC_NAME vs2010)
  elseif(MSVC_VERSION EQUAL 1700)
    set(MSVC_NAME vs2012)
  elseif(MSVC_VERSION EQUAL 1800)
    set(MSVC_NAME vs2013)
  elseif(MSVC_VERSION EQUAL 1900)
    set(MSVC_NAME vs2014)
  elseif(MSVC_VERSION EQUAL 1910)
    set(MSVC_NAME vs2015)
  elseif(MSVC_VERSION EQUAL 1923)
    set(MSVC_NAME vs2019)
  endif()
    
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MSVC_PLATFORM_NAME "x64")
    else()
    set(MSVC_PLATFORM_NAME "x86")
    endif()

    #set(CMAKE_INSTALL_CONFIG_NAME "${MSVC_NAME}-${MSVC_PLATFORM_NAME}-${CMAKE_BUILD_TYPE_INIT}-${MSVC_MT_NAME}")
    #message(STATUS "CMAKE_INSTALL_CONFIG_NAME:${CMAKE_INSTALL_CONFIG_NAME}")

    #set(MSVC_LIB_DIR "${MSVC_NAME}-${MSVC_PLATFORM_NAME}-${CMAKE_BUILD_TYPE_INIT}-${MSVC_MT_NAME}")
    
    set(MSVC_LIB_DIR "${MSVC_NAME}-${MSVC_PLATFORM_NAME}-${CMAKE_BUILD_TYPE}-${MSVC_MT_NAME}")
    set(MSVC_INSTALL_DIR "${MSVC_NAME}-${MSVC_PLATFORM_NAME}-\${CMAKE_INSTALL_CONFIG_NAME}-${MSVC_MT_NAME}")

    set(CMAKE_INSTALL_LIBDIR lib/${MSVC_INSTALL_DIR})
    message(STATUS "CMAKE_INSTALL_LIBDIR:${CMAKE_INSTALL_LIBDIR}")
    
    set(CMAKE_INSTALL_BINDIR bin/${MSVC_INSTALL_DIR})
    message(STATUS "CMAKE_INSTALL_BINDIR:${CMAKE_INSTALL_BINDIR}")
else()
    set(CMAKE_INSTALL_LIBDIR "lib")
endif(MSVC)