LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cpp .cc

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_MODULE := MenuSZK_v2
else
    LOCAL_MODULE := MenuSZK_64
    LOCAL_CXXFLAGS += -DAML64
endif


rwildcard = $(foreach d,$(wildcard $(1)/*),$(call rwildcard,$(d),$(2)) $(filter $(subst *,%,$(2)),$(d)))

LOCAL_SRC_FILES := $(call rwildcard,mod,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,src,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,json,*.cpp)

# $(info ==================== SOURCE FILES ====================)
# $(foreach file,$(LOCAL_SRC_FILES),$(info $(file)))
# $(info ======================================================)

LOCAL_CXXFLAGS += -O2 -DNDEBUG -std=c++17

LOCAL_LDLIBS += -llog

LOCAL_CPPFLAGS += -DOBFUSCATION_KEY=\"$(OBFUSCATION_KEY)\"
# LOCAL_CPPFLAGS += -DOBFUSCATION_KEY=\"$(OBFUSCATION_KEY)\"

include $(BUILD_SHARED_LIBRARY)