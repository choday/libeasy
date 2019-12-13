#https://developer.android.google.cn/ndk/guides/android_mk.html
#ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_PLATFORM=android-29 APP_STL=c++_static
#gtest ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_PLATFORM=android-29 APP_STL=c++_static
#error LOCAL_SHARED_LIBRARIES += $(NDK_ROOT)/sources/third_party/googletest/libs/$(TARGET_ARCH_ABI)/libgoogletest_shared.so
#-fpack-struct=1 googletest segmentation fault
OS_NAME = $(uname -a)

ifeq ($(HOST_OS),windows)
work_dir = $(shell cd)
else
work_dir = $(shell pwd)
endif

THIRD_PARTY_C_INCLUDES=$(NDK_ROOT)/home/include
THIRD_PARTY_LINK_DIR=$(NDK_ROOT)/home/lib/$(APP_ABI)


out_dir		:= $(work_dir)/out/android
TARGET_OBJS := $(out_dir)/$(TARGET_OBJS)
TARGET_OUT  := $(out_dir)/$(TARGET_OUT)


elib_src = $(wildcard src/ebase/c/*.c src/ebase/*.cpp src/ebase/platform/*_linux.cpp)
elib_src += $(wildcard src/eio/*.cpp src/eio/base/*.cpp src/eio/epoll/*.cpp src/eio/ssl/*.cpp src/eio/posix/*.cpp src/eio/platform/*_linux.cpp)

unittest_src = $(wildcard src/ebase/unittest/*.cpp src/eio/unittest/*.cpp)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
#LOCAL_COPY_HEADERS_TO := include/elib 
#LOCAL_COPY_HEADERS := $(wildcard elib/*.cpp eio/*.cpp)
LOCAL_MODULE    := elib_static
LOCAL_SRC_FILES :=  $(elib_src)
#LOCAL_LDLIBS	:=-llog
LOCAL_C_INCLUDES=$(THIRD_PARTY_C_INCLUDES)
LOCAL_EXPORT_C_INCLUDE_DIRS:=
LOCAL_CPP_FEATURES := rtti
include $(BUILD_STATIC_LIBRARY)


LOCAL_PATH := $(work_dir)
include $(CLEAR_VARS)
NDK_APP_DST_DIR := $(work_dir)/out/android/libs/$(TARGET_ARCH_ABI)
LOCAL_MODULE    := elib
LOCAL_LDLIBS	+= -pthread
LOCAL_SRC_FILES :=
LOCAL_STATIC_LIBRARIES += elib_static
LOCAL_CPP_FEATURES := rtti
include $(BUILD_SHARED_LIBRARY)


LOCAL_PATH := $(work_dir)
include $(CLEAR_VARS)
NDK_APP_DST_DIR := $(work_dir)/out/android/libs/$(TARGET_ARCH_ABI)
LOCAL_MODULE    := elib_unittest
LOCAL_LDLIBS	+= -pthread
LOCAL_LDFLAGS   +=
LOCAL_SRC_FILES := $(unittest_src)
LOCAL_C_INCLUDES+= $(work_dir)/src
LOCAL_STATIC_LIBRARIES += elib_static gtest gtest_main
LOCAL_CPP_FEATURES := rtti
include $(BUILD_EXECUTABLE)

$(call import-module,third_party/googletest)


