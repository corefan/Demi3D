
/********************************************************************
    File:       GLTexture.h
    Creator:    demiwangya
*********************************************************************/

#pragma once

#include "Texture.h"

namespace Demi
{
    class DI_GLDRV_API DiGLTextureDrv : public DiTextureDrv
    {
    public:

        DiGLTextureDrv(DiTexture* parent);

        ~DiGLTextureDrv();

    public:

        void                    CreateTexture();

        void                    Release();

        void*                   LockLevel(uint32 level, uint32 surface = 0, DiLockFlag lockflag = LOCK_NORMAL);

        void                    UnlockLevel(uint32 level, uint32 surface = 0);

        void                    Bind(uint32 samplerIndex);

        void                    CopyToMemory(const DiBox &srcBox, const DiPixelBox &dst);

        void                    CopyFromMemory(const DiPixelBox &srcBox, const DiBox &dst, uint32 level, uint32 surface = 0);

        void*                   GetSurfaceHandle();

    private:

        GLenum                  mGLFormat;

        GLenum                  mGLTextureType;

        GLuint                  mTextureID;

        uint32                  mImageSize;

        void*                   mLockedBuffer;
    };
}