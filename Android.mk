LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cpp .cc

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_MODULE := MenuSZK_v2
else
    LOCAL_MODULE := MenuSZK_64
    LOCAL_CXXFLAGS += -DAML64
endif


LOCAL_SRC_FILES := $(wildcard mod/*.cpp)
LOCAL_SRC_FILES += $(wildcard src/*.cpp)
LOCAL_SRC_FILES += $(wildcard src/utils/*.cpp)
LOCAL_SRC_FILES += $(wildcard src/container/*.cpp)
LOCAL_SRC_FILES += $(wildcard src/keyboard/*.cpp)
LOCAL_SRC_FILES += $(wildcard src/window/*.cpp)
LOCAL_SRC_FILES += $(wildcard src/window/menuItems/*.cpp)
LOCAL_SRC_FILES += $(wildcard src/webServer/*.cpp)
LOCAL_SRC_FILES += $(wildcard src/cellphone/*.cpp)
LOCAL_SRC_FILES += $(wildcard src/bottomPanel/*.cpp)
LOCAL_SRC_FILES += $(wildcard src/screenDebug/*.cpp)
LOCAL_SRC_FILES += $(wildcard json/*.cpp)

LOCAL_CXXFLAGS += -O2 -DNDEBUG -std=c++17

LOCAL_LDLIBS += -llog

LOCAL_CPPFLAGS += -DOBFUSCATION_KEY=\"$(OBFUSCATION_KEY)\"
# LOCAL_CPPFLAGS += -DOBFUSCATION_KEY=\"$(OBFUSCATION_KEY)\"

include $(BUILD_SHARED_LIBRARY)