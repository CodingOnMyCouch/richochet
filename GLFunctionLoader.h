//
// Created by gormlai on 11/06/2016.
//

#ifndef RICOCHET_GLFUNCTIONLOADER_H
#define RICOCHET_GLFUNCTIONLOADER_H

#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#if defined(_ANDROID_)
class GLFunctionLoader {
public:
    static GLFunctionLoader & instance();
/*
    PFNGLBINDVERTEXARRAYOESPROC bindVertexArrayOES;
    PFNGLDELETEVERTEXARRAYSOESPROC deleteVertexArraysOES;
    PFNGLGENVERTEXARRAYSOESPROC genVertexArraysOES;
    PFNGLISVERTEXARRAYOESPROC isVertexArrayOES;
*/
private:
    GLFunctionLoader();




};

#endif

#endif //RICOCHET_GLFUNCTIONLOADER_H
