LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := curl_static

LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/lib/*.c)
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/lib/*.c)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/lib/curlx/*.c)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/lib/vauth/*.c)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/lib/vdns/*.c)

LOCAL_SRC_FILES := $(filter-out \
    $(LOCAL_PATH)/lib/curl_fopen.c \
    $(LOCAL_PATH)/lib/file.c \
    $(LOCAL_PATH)/lib/fileinfo.c \
    $(LOCAL_PATH)/lib/ldap.c \
    $(LOCAL_PATH)/lib/imap.c \
    $(LOCAL_PATH)/lib/pop3.c \
    $(LOCAL_PATH)/lib/smtp.c \
    $(LOCAL_PATH)/lib/telnet.c \
    $(LOCAL_PATH)/lib/tftp.c \
    $(LOCAL_PATH)/lib/ftp.c \
    $(LOCAL_PATH)/lib/gopher.c \
    $(LOCAL_PATH)/lib/dict.c \
    $(LOCAL_PATH)/lib/mqtt.c \
    $(LOCAL_PATH)/lib/rtsp.c \
    $(LOCAL_PATH)/lib/smb.c, \
    $(LOCAL_SRC_FILES))

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/lib

LOCAL_CFLAGS += \
    -DCURL_STATICLIB \
    -DCURL_DISABLE_SSL \
    -DSIZEOF_CURL_OFF_T=8 \
    -Dsread=read \
    -Dswrite=write \
    -O2 \
    -fPIC

LOCAL_CFLAGS += -include unistd.h
LOCAL_CFLAGS += -include netinet/in.h
LOCAL_CFLAGS += -include netdb.h



include $(BUILD_STATIC_LIBRARY)