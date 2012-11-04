LOCAL_PATH := $(call my-dir)
TEST_DIRECTORY := 12-4

include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := libopenal
OPENALTOP := $(LOCAL_PATH)/../klaatu-openal-soft
SRC_FILES := $(OPENALTOP)/Alc/ALc.c $(OPENALTOP)/Alc/alcConfig.c \
    $(OPENALTOP)/Alc/alcEcho.c $(OPENALTOP)/Alc/alcModulator.c \
    $(OPENALTOP)/Alc/alcReverb.c $(OPENALTOP)/Alc/alcRing.c \
    $(OPENALTOP)/Alc/alcThread.c $(OPENALTOP)/Alc/ALu.c \
    $(OPENALTOP)/Alc/bs2b.c $(OPENALTOP)/Alc/null.c \
    $(OPENALTOP)/OpenAL32/alAuxEffectSlot.c $(OPENALTOP)/OpenAL32/alBuffer.c \
    $(OPENALTOP)/OpenAL32/alDatabuffer.c $(OPENALTOP)/OpenAL32/alEffect.c \
    $(OPENALTOP)/OpenAL32/alError.c $(OPENALTOP)/OpenAL32/alExtension.c \
    $(OPENALTOP)/OpenAL32/alFilter.c $(OPENALTOP)/OpenAL32/alListener.c \
    $(OPENALTOP)/OpenAL32/alSource.c $(OPENALTOP)/OpenAL32/alState.c \
    $(OPENALTOP)/OpenAL32/alThunk.c
LOCAL_C_INCLUDES += external/klaatu-openal-soft/android/jni
LOCAL_C_INCLUDES += external/klaatu-openal-soft/include external/klaatu-openal-soft/OpenAL32/Include
LOCAL_SRC_FILES := $(SRC_FILES:$(LOCAL_PATH)/%=%) ../klaatu-api/klaatu_audio.cpp ../klaatu-gfx-extra/openalandroid.c
LOCAL_LDLIBS = -llog
LOCAL_CFLAGS := -O3 -mno-thumb -DAL_ALEXT_PROTOTYPES
LOCAL_CPPFLAGS := -O3 -mno-thumb
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := liblog libmedia libbinder libcutils libutils

# in 2.3, the path in 'build/core/combo/TARGET_linux-arm.mk' mistakenly includes 'arch/' !!!
LOCAL_C_INCLUDES += bionic/libm/include/arm external/klaatu-api
LOCAL_PRELINK_MODULE = false
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := foobar
LOCAL_C_INCLUDES = external/klaatu-openal-soft/include
LOCAL_C_INCLUDES += external/stlport/stlport bionic frameworks/native/include frameworks/base/services $(LOCAL_PATH)
LOCAL_C_INCLUDES += frameworks/base/include/surfaceflinger frameworks/native/include/gui \
    frameworks/base/services/input frameworks/base/include/ui external/klaatu-api external/klaatu-gfx external/klaatu-gfx-extra
LOCAL_C_INCLUDES += external/libpng external/klaatu-recastnavigation/Detour/Include external/klaatu-recastnavigation/Recast/Include
LOCAL_C_INCLUDES += external/klaatu-recastnavigation/Detour/Include external/klaatu-recastnavigation/Recast/Include external/klaatu-recastnavigation/DebugUtils/Include
SRC_FILES := $(wildcard $(LOCAL_PATH)/../klaatu-gfx/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-gfx-extra/bullet/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-gfx-extra/ttf/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-gfx-extra/nvtristrip/*.cpp)
LOCAL_C_INCLUDES += external/klaatu-gfx-extra/zlib
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-gfx-extra/zlib/*.c)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-recastnavigation/Detour/Source/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-recastnavigation/Recast/Source/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../klaatu-gfx-extra/vorbis/*.c)
SRC_FILES := $(SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(SRC_FILES) sdk/$(TEST_DIRECTORY)/templateApp.cpp \
    ../klaatu-api/klaatu_api.cpp ../klaatu-api/klaatu_touch.cpp \
    ../klaatu-api/klaatu_sensor.cpp ../klaatu-api/klaatu_audio.cpp
LOCAL_SHARED_LIBRARIES := libopenal libstlport libEGL libGLESv2 liblog \
        libutils libbinder libgui
ifeq ($(PLATFORM_VERSION),2.3.3)
LOCAL_SHARED_LIBRARIES += libsurfaceflinger_client libui
else
LOCAL_SHARED_LIBRARIES += libinput
endif
LOCAL_SHARED_LIBRARIES += libandroid_runtime
LOCAL_SHARED_LIBRARIES += libmedia libcutils
LOCAL_STATIC_LIBRARIES += libpng

LOCAL_LDLIBS := -L$(LOCAL_PATH)/../libs/armeabi 
LOCAL_LDLIBS += -lz -lm -ldl -lGLESv2 -lEGL -llog -lopenal
LOCAL_CFLAGS := -O3 -mno-thumb -Wno-write-strings
SVERSION:=$(subst ., ,$(PLATFORM_VERSION))
LOCAL_CFLAGS += -DSHORT_PLATFORM_VERSION=$(word 1,$(SVERSION))$(word 2,$(SVERSION))
LOCAL_CPPFLAGS := $(LOCAL_CFLAGS)
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

#
# now compile asset files into .apk for use by test program
#
PRIVATE_ASSET_DIR := $(LOCAL_PATH)/sdk/$(TEST_DIRECTORY)/assets
PRIVATE_RESOURCE_DIR := $(LOCAL_PATH)/res
PRIVATE_ANDROID_MANIFEST := $(LOCAL_PATH)/AndroidManifest.xml

all_assets := $(addprefix $(PRIVATE_ASSET_DIR)/,$(patsubst assets/%,%,$(call find-subdir-assets,$(PRIVATE_ASSET_DIR))))
all_resources := $(addprefix $(PRIVATE_RESOURCE_DIR)/, $(patsubst res/%,%, $(call find-subdir-assets,$(PRIVATE_RESOURCE_DIR))))
#THISAPK := $(intermediates)/package.apk
THISAPK := $(LOCAL_INSTALLED_MODULE).apk

$(THISAPK): $(all_assets) $(all_resources) $(PRIVATE_ANDROID_MANIFEST) $(AAPT) $(LOCAL_PATH)/Android.mk
	echo "target APKPackage: $(PRIVATE_MODULE) ($@)"
	$(create-empty-package)
	$(add-assets-to-package)

$(LOCAL_BUILT_MODULE): $(THISAPK)
