#-----------------------------
# Framework lib 

# set local path for lib
LOCAL_PATH := $(call my-dir)

DAVA_ROOT := $(LOCAL_PATH)


# clear all variables
include $(CLEAR_VARS)

# set module name
LOCAL_MODULE := libInternal

# set path for includes
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Libs/include

# set exported includes
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

# set source files 

LOCAL_SRC_FILES := \
                     $(subst $(LOCAL_PATH)/,, \
                     $(wildcard $(LOCAL_PATH)/Animation/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Base/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Collision/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Core/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Database/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Debug/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Entity/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/FileSystem/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Input/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Math/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Math/Neon/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Network/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Particles/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Platform/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Platform/Android/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Render/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Render/2D/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Render/3D/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Render/Effects/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Scene2D/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Scene3D/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Sound/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/UI/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/Utils/*.cpp) \
                     $(wildcard $(LOCAL_PATH)/DLC/*.cpp))



# set build flags
LOCAL_CFLAGS := -frtti -g -O2 -DGL_GLEXT_PROTOTYPES=1 -Wno-psabi

# set exported build flags
LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)

# set used libs

LIBS_PATH := $(call host-path,$(LOCAL_PATH)/../../Libs/libs)

LOCAL_LDLIBS := -lGLESv1_CM -llog -lGLESv2 -lEGL
LOCAL_LDLIBS += $(LIBS_PATH)/libxml_android.a
LOCAL_LDLIBS += $(LIBS_PATH)/libpng_android.a
LOCAL_LDLIBS += $(LIBS_PATH)/libfreetype_android.a
LOCAL_LDLIBS += $(LIBS_PATH)/libyaml_android.a
LOCAL_LDLIBS += $(LIBS_PATH)/libmongodb_android.a
LOCAL_LDLIBS += $(LIBS_PATH)/libdxt_android.a
LOCAL_LDLIBS += $(LIBS_PATH)/libcurl_android.a
LOCAL_LDLIBS += -fuse-ld=gold -fno-exceptions

# set exported used libs
LOCAL_EXPORT_LDLIBS := $(LOCAL_LDLIBS)

# set arm mode
# LOCAL_ARM_MODE := arm


# set included libraries
LOCAL_STATIC_LIBRARIES := libbox2d
LOCAL_STATIC_LIBRARIES += android_native_app_glue

include $(BUILD_STATIC_LIBRARY)

# include modules
$(call import-add-path,$(DAVA_ROOT)/..)
$(call import-add-path,$(DAVA_ROOT)/../External)
$(call import-add-path,$(DAVA_ROOT)/../External/Box2D)
$(call import-add-path,$(DAVA_ROOT))

$(call import-module,box2d)
$(call import-module,android/native_app_glue)
