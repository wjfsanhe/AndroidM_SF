/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define ATRACE_TAG ATRACE_TAG_GRAPHICS

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <ui/Rect.h>

#include <utils/String8.h>
#include <utils/Trace.h>

#include <cutils/compiler.h>
#include <gui/ISurfaceComposer.h>
#include <math.h>

#include "GLES20RenderEngine.h"
#include "Program.h"
#include "ProgramCache.h"
#include "Description.h"
#include "Mesh.h"
#include "Texture.h"
#include "../DisplayDevice.h"
#include <SkBitmap.h>
#include <SkStream.h>
#include <SkImageDecoder.h>
#include "sdk/Base/MojingMath.h"
#include <ui/mat4.h>
#include "Layer.h"
#include "MojingVRRomAPI.h"
using namespace Baofeng::Mojing;
// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------
static float* g_pfViewMatrix = NULL;
static mat4 g_ViewMat4;

GLES20RenderEngine::GLES20RenderEngine() :
        mVpWidth(0), mVpHeight(0) {
    baofeng_initVR();
    isInitDistortion = false;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mMaxTextureSize);
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, mMaxViewportDims);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);

    const uint16_t protTexData[] = { 0 };
    glGenTextures(1, &mProtectedTexName);
    glBindTexture(GL_TEXTURE_2D, mProtectedTexName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0,
            GL_RGB, GL_UNSIGNED_SHORT_5_6_5, protTexData);

    //mColorBlindnessCorrection = M;
    uint32_t textureName;
    glGenTextures(1,  &textureName);
    glBindTexture(GL_TEXTURE_2D, textureName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    SkBitmap bitmap;
    char path[256] = "/system/media/baofeng_cursor.png";
    bool ret = SkImageDecoder::DecodeFile(path, &bitmap);
    bitmap.lockPixels();
    const int w = bitmap.width();
    const int h = bitmap.height();
    const void* p = bitmap.getPixels();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,p);
	mHeadTrackTexture.init(Texture::TEXTURE_2D, textureName);
    mHeadTrackTexture.setDimensions(w, h);
}

GLES20RenderEngine::~GLES20RenderEngine() {
    if (mHeadTrackTexture.getTextureName()) {
        uint32_t textureName = mHeadTrackTexture.getTextureName();
        glDeleteTextures(1, &textureName);
    }
}


size_t GLES20RenderEngine::getMaxTextureSize() const {
    return mMaxTextureSize;
}

size_t GLES20RenderEngine::getMaxViewportDims() const {
    return
        mMaxViewportDims[0] < mMaxViewportDims[1] ?
            mMaxViewportDims[0] : mMaxViewportDims[1];
}

void GLES20RenderEngine::setViewportAndProjection(
        size_t vpw, size_t vph, Rect sourceCrop, size_t hwh, bool yswap,
        Transform::orientation_flags rotation) {

    size_t l = sourceCrop.left;
    size_t r = sourceCrop.right;

    // In GL, (0, 0) is the bottom-left corner, so flip y coordinates
    size_t t = hwh - sourceCrop.top;
    size_t b = hwh - sourceCrop.bottom;

    mat4 m;
    if (yswap) {
        m = mat4::ortho(l, r, t, b, 0, 1);
    } else {
        m = mat4::ortho(l, r, b, t, 0, 1);
    }

    // Apply custom rotation to the projection.
    float rot90InRadians = 2.0f * static_cast<float>(M_PI) / 4.0f;
    switch (rotation) {
        case Transform::ROT_0:
            break;
        case Transform::ROT_90:
            m = mat4::rotate(rot90InRadians, vec3(0,0,1)) * m;
            break;
        case Transform::ROT_180:
            m = mat4::rotate(rot90InRadians * 2.0f, vec3(0,0,1)) * m;
            break;
        case Transform::ROT_270:
            m = mat4::rotate(rot90InRadians * 3.0f, vec3(0,0,1)) * m;
            break;
        default:
            break;
    }

    glViewport(0, 0, vpw, vph);
    mState.setProjectionMatrix(m);
    mVpWidth = vpw;
    mVpHeight = vph;
}

void GLES20RenderEngine::setupLayerBlending(
    bool premultipliedAlpha, bool opaque, int alpha) {

    mState.setPremultipliedAlpha(premultipliedAlpha);
    mState.setOpaque(opaque);
    mState.setPlaneAlpha(alpha / 255.0f);

    if (alpha < 0xFF || !opaque) {
        glEnable(GL_BLEND);
        glBlendFunc(premultipliedAlpha ? GL_ONE : GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

void GLES20RenderEngine::setupDimLayerBlending(int alpha) {
    mState.setPlaneAlpha(1.0f);
    mState.setPremultipliedAlpha(true);
    mState.setOpaque(false);
    mState.setColor(0, 0, 0, alpha/255.0f);
    mState.disableTexture();

    if (alpha == 0xFF) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void GLES20RenderEngine::setupLayerTexturing(const Texture& texture) {
    GLuint target = texture.getTextureTarget();
    glBindTexture(target, texture.getTextureName());
    GLenum filter = GL_NEAREST;
    if (texture.getFiltering()) {
        filter = GL_LINEAR;
    }
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);

    mState.setTexture(texture);
}

void GLES20RenderEngine::setupLayerBlackedOut() {
    glBindTexture(GL_TEXTURE_2D, mProtectedTexName);
    Texture texture(Texture::TEXTURE_2D, mProtectedTexName);
    texture.setDimensions(1, 1); // FIXME: we should get that from somewhere
    mState.setTexture(texture);
}

mat4 GLES20RenderEngine::setupColorTransform(const mat4& colorTransform) {
    mat4 oldTransform = mState.getColorMatrix();
    mState.setColorMatrix(colorTransform);
    return oldTransform;
}

void GLES20RenderEngine::disableTexturing() {
    mState.disableTexture();
}

void GLES20RenderEngine::disableBlending() {
    glDisable(GL_BLEND);
}


void GLES20RenderEngine::bindImageAsFramebuffer(EGLImageKHR image,
        uint32_t* texName, uint32_t* fbName, uint32_t* status) {
    GLuint tname, name;
    // turn our EGLImage into a texture
    glGenTextures(1, &tname);
    glBindTexture(GL_TEXTURE_2D, tname);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, (GLeglImageOES)image);

    // create a Framebuffer Object to render into
    glGenFramebuffers(1, &name);
    glBindFramebuffer(GL_FRAMEBUFFER, name);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tname, 0);

    *status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    *texName = tname;
    *fbName = name;
}

void GLES20RenderEngine::unbindFramebuffer(uint32_t texName, uint32_t fbName) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbName);
    glDeleteTextures(1, &texName);
}

void GLES20RenderEngine::setupFillWithColor(float r, float g, float b, float a) {
    mState.setPlaneAlpha(1.0f);
    mState.setPremultipliedAlpha(true);
    mState.setOpaque(false);
    mState.setColor(r, g, b, a);
    mState.disableTexture();
    glDisable(GL_BLEND);
}

void GLES20RenderEngine::drawMesh(const Mesh& mesh) {

    ProgramCache::getInstance().useProgram(mState);

    if (mesh.getTexCoordsSize()) {
        glEnableVertexAttribArray(Program::texCoords);
        glVertexAttribPointer(Program::texCoords,
                mesh.getTexCoordsSize(),
                GL_FLOAT, GL_FALSE,
                mesh.getByteStride(),
                mesh.getTexCoords());
    }

    glVertexAttribPointer(Program::position,
            mesh.getVertexSize(),
            GL_FLOAT, GL_FALSE,
            mesh.getByteStride(),
            mesh.getPositions());

    glDrawArrays(mesh.getPrimitive(), 0, mesh.getVertexCount());

    if (mesh.getTexCoordsSize()) {
        glDisableVertexAttribArray(Program::texCoords);
    }
}

void GLES20RenderEngine::drawMesh(const Mesh& mesh, int mode) {
    float scaleRate = 1; //default
    int w,h;
    if (mGroupStack.isEmpty()) {
        w = 2880;
        h = 1440;
    } else {
        Group group(mGroupStack.top());
        w = group.width;
        h = group.height;
    }
    if(w>h)//portrait
    {
        if(Layer::eRenderLeft2D == mode) { //left
            glViewport(0+w/2*(1-scaleRate)/2, 0+h*(1-scaleRate)/2, w/2*scaleRate, h*scaleRate);
            drawMesh_ex(mesh,false);
        } else if(Layer::eRenderRight2D == mode) { //right
            glViewport(w/2+w/2*(1-scaleRate)/2, 0+h*(1-scaleRate)/2, w/2*scaleRate, h*scaleRate);
            drawMesh_ex(mesh,false);
        } else if(Layer::eRenderLeft3D == mode) {
            glViewport(0+w/2*(1-scaleRate)/2, 0+h*(1-scaleRate)/2, w/2*scaleRate, h*scaleRate);
            drawMesh_ex(mesh,true);
        }else if(Layer::eRenderRight3D == mode) {
            glViewport(w/2+w/2*(1-scaleRate)/2, 0+h*(1-scaleRate)/2, w/2*scaleRate, h*scaleRate);
            drawMesh_ex(mesh,true);
        }
    }
#if 0 //portrait modes
    else {
        if(1 == mode) {
            glViewport(0+w*(1-scaleRate)/2, 0+h/2*(1-scaleRate)/2, w*scaleRate, h/2*scaleRate);
            drawMesh_ex(mesh);
        } else if(2 == mode) {
            glViewport(0+w*(1-scaleRate)/2, h/2+h/2*(1-scaleRate)/2, w*scaleRate, h/2*scaleRate);
            drawMesh_ex(mesh);
        }
    }
#endif
}

void GLES20RenderEngine::drawHeadTrack() {

    if (mHeadTrackTexture.getTextureName()) {

        glBindTexture(GL_TEXTURE_2D, mHeadTrackTexture.getTextureName());
        mState.setTexture(mHeadTrackTexture);
        mState.setPlaneAlpha(1.0f);
        mState.setPremultipliedAlpha(true);
        mState.setOpaque(false);

        mat4 mat;
        mat[0][0] = 0.5;
        mat[1][1] = 0.5;
        mState.setProjectionMatrix(mat);

        glEnable(GL_BLEND);

        Group group(mGroupStack.top());
        int width = group.width/2;
        int height = group.height;
        //float width = (mVpWidth>mVpHeight ? mVpWidth : mVpHeight)/2;
        //float height = (mVpWidth<mVpHeight ? mVpWidth : mVpHeight);

        Mesh mesh(Mesh::TRIANGLE_FAN, 4, 2, 2);
        Mesh::VertexArray<vec2> position(mesh.getPositionArray<vec2>());
        Mesh::VertexArray<vec2> texCoord(mesh.getTexCoordArray<vec2>());

        position[0] = vec2(- (float)mHeadTrackTexture.getWidth()/width, - (float)mHeadTrackTexture.getHeight()/height);
        position[1] = vec2(  (float)mHeadTrackTexture.getWidth()/width, - (float)mHeadTrackTexture.getHeight()/height);
        position[2] = vec2(  (float)mHeadTrackTexture.getWidth()/width,   (float)mHeadTrackTexture.getHeight()/height);
        position[3] = vec2(- (float)mHeadTrackTexture.getWidth()/width,   (float)mHeadTrackTexture.getHeight()/height);

        texCoord[0] = vec2(0, 0);
        texCoord[1] = vec2(1, 0);
        texCoord[2] = vec2(1, 1);
        texCoord[3] = vec2(0, 1);

        glViewport(0, 0, width, height);

        drawMesh(mesh);
        glViewport(width, 0, width, height);
        drawMesh(mesh);
    }

}

void GLES20RenderEngine::drawDistortion()
{
    drawHeadTrack();
    glViewport(0, 0, mVpWidth, mVpHeight);

    if(!isInitDistortion) {
        //FILE* pFile = fopen("/sdcard/MojingSDK/DDD2.dat", "rb");
        FILE* pFile = fopen("/system/media/DDD2.dat", "rb");
        if (pFile)
        {
            fseek(pFile, 0, SEEK_END);
            int iBufferSize = ftell(pFile);
            fseek(pFile, 0, SEEK_SET);
            void * pBuffer = malloc(iBufferSize);
            fread(pBuffer, 1, iBufferSize, pFile);
            fclose(pFile);
            //bNeedDeleteBuffer = true;
            MojingVRRomAPI_CreateDistortion(pBuffer, iBufferSize);
            isInitDistortion = true;
        }
    }
    Group group(mGroupStack.top());
    mGroupStack.pop();
    // activate the previous render targt
    GLuint fbo = 0;
    if (!mGroupStack.isEmpty()) {
        fbo = mGroupStack.top().fbo;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, mVpWidth, mVpHeight);
    // set our state
    Texture texture(Texture::TEXTURE_2D, group.texture);
    //texture.setDimensions(group.width, group.height);
    glBindTexture(GL_TEXTURE_2D, group.texture);
    //push group
    bool retDraw = MojingVRRomAPI_DrawDistortion(group.texture, NULL, NULL);
    glFinish();
    // free our fbo and texture
    glDeleteFramebuffers(1, &group.fbo);
    glDeleteTextures(1, &group.texture);

}

void GLES20RenderEngine::drawMesh_ex(const Mesh& mesh,bool is3DLayer) {
    Matrix4f pRot;
    //90
    Matrix4f pRotView(0.0,1.0,0.0,0.0,
            -1.0,-0.0,0.0,0.0,
            0.0,0.0,1.0,0.0,
            0.0,0.0,0.0,1.0);

    Matrix4f pProject;
    mat4 matProject = mat4::frustum(-1,1,-1,1, 1, 6000);
    for(int i=0;i<16;i++)
                pProject.M[i/4][i%4] = matProject[i/4][i%4];

    Matrix4f pMVP;
    if(g_pfViewMatrix != NULL)
    {
        for(int i=0;i<16;i++)
            pRot.M[i/4][i%4]= g_pfViewMatrix[i];
    }
    if(is3DLayer)
        pMVP = pRot*pRotView*pProject;
    else
        pMVP = pRot*pRotView*pProject;

    mat4 m;
    for(int i=0;i<16;i++)
        m[i/4][i%4]= pMVP.M[i/4][i%4];
    mState.setProjectionMatrix(m);

    ProgramCache::getInstance().useProgram(mState);

    if (mesh.getTexCoordsSize()) {
        glEnableVertexAttribArray(Program::texCoords);
        glVertexAttribPointer(Program::texCoords,
                mesh.getTexCoordsSize(),
                GL_FLOAT, GL_FALSE,
                mesh.getByteStride(),
                mesh.getTexCoords());
    }

    glVertexAttribPointer(Program::position,
            mesh.getVertexSize(),
            GL_FLOAT, GL_FALSE,
            mesh.getByteStride(),
            mesh.getPositions());

    glDrawArrays(mesh.getPrimitive(), 0, mesh.getVertexCount());
    int num = mesh.getVertexCount();
    const float* position_debug = mesh.getPositions();
    if (mesh.getTexCoordsSize()) {
        glDisableVertexAttribArray(Program::texCoords);
    }

}

void GLES20RenderEngine::baofeng_initVR(){
    mBfVR.dpyId=0;
    mBfVR.startFlag=true;
    for(int dpy=0;dpy<DISPLAY_NUM;dpy++){
        mBfVR.tmpFbo[dpy]=0;
        mBfVR.tmpTex[dpy]=0;
        mBfVR.tmpFboWidth[dpy]  = mVpWidth>mVpHeight?mVpWidth:mVpHeight;//force tmpBuffer  landscape  Mode for sdk distortion
        mBfVR.tmpFboHeight[dpy] = mVpWidth<mVpHeight?mVpWidth:mVpHeight;
    }
}

void GLES20RenderEngine::baofeng_createFBO(int dpyId){
    mBfVR.dpyId = dpyId;
    int width = mBfVR.tmpFboWidth[dpyId];
    int height = mBfVR.tmpFboHeight[dpyId];
    /*create tmp FBO*/
    glGenTextures(1, &mBfVR.tmpTex[dpyId]);
    glBindTexture(GL_TEXTURE_2D, mBfVR.tmpTex[dpyId]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,0);
    glGenFramebuffers(1, &mBfVR.tmpFbo[dpyId]);
    glBindFramebuffer(GL_FRAMEBUFFER, mBfVR.tmpFbo[dpyId]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBfVR.tmpTex[dpyId], 0);
}

void GLES20RenderEngine::bindTmpFBO( ){
        GLuint tname, name;
        // create the texture
        glGenTextures(1, &tname);
        glBindTexture(GL_TEXTURE_2D, tname);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mVpWidth, mVpHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        int w = mVpWidth>mVpHeight ? mVpWidth : mVpHeight;
        int h = mVpWidth<mVpHeight ? mVpWidth : mVpHeight;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        //glClearColor(0,0,0,1);

        // create a Framebuffer Object to render into
        glGenFramebuffers(1, &name);
        glBindFramebuffer(GL_FRAMEBUFFER, name);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tname, 0);

        Group group;
        group.texture = tname;
        group.fbo = name;
        group.width = w;
        group.height = h;
        mGroupStack.push(group);
}

void GLES20RenderEngine::bindDisplayFBO( ){
    Group group(mGroupStack.top());
    mGroupStack.pop();
    // activate the previous render targt
    GLuint fbo = 0;
    if (!mGroupStack.isEmpty()) {
        fbo = mGroupStack.top().fbo;
    }
    //glViewPort(0, 0, mVpWidth, mVpHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // set our state
    Texture texture(Texture::TEXTURE_2D, group.texture);
    //texture.setDimensions(group.width, group.height);
    glBindTexture(GL_TEXTURE_2D, group.texture);
    // free our fbo and texture
    glDeleteFramebuffers(1, &group.fbo);
    glDeleteTextures(1, &group.texture);

    //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

int GLES20RenderEngine::loadSkyboxTex( ){
    glGenTextures(1, &mBfVR.skyboxTex[mBfVR.dpyId]);
    glBindTexture(GL_TEXTURE_2D, mBfVR.skyboxTex[mBfVR.dpyId]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    SkBitmap bitmap;
    char path[256] = "/system/media/skybox.png";
    bool ret = SkImageDecoder::DecodeFile(path, &bitmap);
    bitmap.lockPixels();
    const int w = bitmap.width();
    const int h = bitmap.height();
    const void* p = bitmap.getPixels();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,p);
    return  mBfVR.skyboxTex[mBfVR.dpyId];
}


void GLES20RenderEngine::setViewMatrix(float* pfViewMatrix)
{
    if(pfViewMatrix != NULL) {
        if(g_pfViewMatrix==NULL)
            g_pfViewMatrix = (float*)malloc(16*sizeof(float));
        memcpy(g_pfViewMatrix, pfViewMatrix, 16*sizeof(float));
        mat4 m;
        for(int i=0;i<16;i++)
            g_ViewMat4[i/4][i%4] = g_pfViewMatrix[i];
    }
}

void GLES20RenderEngine::drawSkybox(){
    /*
       enum MODEL_TYPE
       {
       MODEL_SKYBOX     = 0,
       MODEL_SPHERE_2D      = 1,
       MODEL_SPHERE_3D_UD   = 2,
       MODEL_SPHERE_3D_LR   = 3,
       MODEL_RECT_2D        = 4
       };
       enum TEXTURE_TYPE
       {
       TextureType_DEFAULT = 0 ,
       TextureType_MOVIE = 1
       };
       */
    static int iModelKey = MojingVRRomAPI_CreateModel(0 ,0);
    //iEye  0 L  1 R
    Matrix4f pRot;
    Matrix4f pRotView(0.0,1.0,0.0,0.0,
                      -1.0,-0.0,0.0,0.0,
                      0.0,0.0,1.0,0.0,
                      0.0,0.0,0.0,1.0);
    Matrix4f pProject;
    mat4 matProject = mat4::frustum(-1,1,-1,1, 1, 2000);
    for(int i=0;i<16;i++)
                pProject.M[i/4][i%4] = matProject[i/4][i%4];
    Matrix4f pMVP;

    if(g_pfViewMatrix != NULL)
    {
        for(int i=0;i<16;i++)
            pRot.M[i/4][i%4]= g_pfViewMatrix[i];
    }

    pMVP = pRot*pRotView*pProject;
    //pMVP =  pProject;
    //near 1   far  R=20
    static int iTextureId = -1;

    int w ,h;
    if(-1 == iTextureId)
        iTextureId = loadSkyboxTex();
    if (!mGroupStack.isEmpty()) {
        Group group(mGroupStack.top());
        w = group.width;
        h = group.height;
    } else {
        w = 2880;
        h = 1440;
    }
    glViewport(0, 0, w/2, h);
    MojingVRRomAPI_DrawModel(iModelKey, 0, iTextureId, &pMVP.M[0][0]);
    glViewport(w/2, 0, w/2, h);
    MojingVRRomAPI_DrawModel(iModelKey, 1, iTextureId, &pMVP.M[0][0]);
    //glViewport(0, 0, mVpWidth, mVpHeight);
    glViewport(0, 0, w, h);

}

void GLES20RenderEngine::dump(String8& result) {
    RenderEngine::dump(result);
}

// ---------------------------------------------------------------------------
}; // namespace android
// ---------------------------------------------------------------------------

#if defined(__gl_h_)
#error "don't include gl/gl.h in this file"
#endif
