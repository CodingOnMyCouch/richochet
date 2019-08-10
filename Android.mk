LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_CPPFLAGS += -fexceptions -std=c++11 -Wall -DGLEW_NO_GLU -D_ANDROID_ -DGL_GLEXT_PROTOTYPES

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
                    $(LOCAL_PATH)/../../../../../common/include

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
    main.cpp BallObject.cpp Game.cpp GameLevel.cpp GameObject.cpp \
    GLFunctionLoader.cpp ParticleGen.cpp PostProcessor.cpp ResourceManager.cpp \
    Shader.cpp SpriteRenderer.cpp TextRenderer.cpp Texture.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer
LOCAL_STATIC_LIBRARIES := freetype2-static

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv3 -llog

include $(BUILD_SHARED_LIBRARY)
