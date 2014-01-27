
/********************************************************************
    File:       GLFrameBuffer.h
    Creator:    demiwangya
*********************************************************************/

#include "Texture.h"

namespace Demi
{
    class DI_GLDRV_API DiGLFrameBuffer
    {
    public:

        DiGLFrameBuffer();

        ~DiGLFrameBuffer();

    public:

        void            AttachSurface(uint32 attachment, DiTexturePtr surface);

        void            DetarchSurface(uint32 attachment);

        void            Bind();

        void            SwapBuffers();

        void            AttachDepthBuffer(DiDepthBuffer* depthBuffer);

        void            DetachDepthBuffer();

        DiPixelFormat   GetFormat();

    private:

        void            Init();

    private:

        GLuint          mFBOId;

        DiTexturePtr    mColorBuffer[MAX_MRT];
    };

    //////////////////////////////////////////////////////////////////////////

    class DiGLFBOManager
    {
    public:

        DiGLFBOManager(bool atimode);

        ~DiGLFBOManager();

    public:

        void                GetBestDepthStencil(GLenum internalFormat, GLenum *depthFormat, GLenum *stencilFormat);

    private:

        void                DetectFBOFormats();

        GLuint              TryFormat(GLenum depthFormat, GLenum stencilFormat);

        bool                TryPackedFormat(GLenum packedFormat);

    private:

        struct FormatProperties
        {
            bool valid; // This format can be used as RTT (FBO)

            struct Mode
            {
                size_t depth;     // Depth format (0=no depth)
                size_t stencil;   // Stencil format (0=no stencil)
            };

            DiVector<Mode> modes;
        };

        FormatProperties    mProps[PIXEL_FORMAT_MAX];

        bool                mATIMode;

        GLuint              mTempFBO;
    };
}