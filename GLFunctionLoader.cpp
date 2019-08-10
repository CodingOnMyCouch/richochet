//
// Created by gormlai on 11/06/2016.
//

#if defined(_ANDROID_)
#include "GLFunctionLoader.h"
#include <dlfcn.h>
#include <android/log.h>

GLFunctionLoader & GLFunctionLoader::instance()
{
    static GLFunctionLoader inst;
    return inst;
}

GLFunctionLoader::GLFunctionLoader()
{
/*
    void *libhandle = dlopen("libGLESv2.so", RTLD_LAZY);

    bindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC) dlsym(libhandle, "glBindVertexArrayOES");
    deleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC) dlsym(libhandle, "glDeleteVertexArraysOES");
    genVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)dlsym(libhandle, "glGenVertexArraysOES");
    isVertexArrayOES = (PFNGLISVERTEXARRAYOESPROC)dlsym(libhandle, "glIsVertexArrayOES");

    __android_log_print(ANDROID_LOG_INFO, "GLFunctionLoader", "bindVertexArray=%p\n", bindVertexArrayOES);
*/
}

#endif