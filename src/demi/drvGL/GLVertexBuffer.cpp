
#include "GLVertexBuffer.h"
#include "GLDriver.h"
#include "GLBufferManager.h"
#include "GLTypeMappings.h"

namespace Demi
{
    DiGLVertexBuffer::DiGLVertexBuffer()
        : mBufferId(0)
        , mLockingScratch(nullptr)
        , mLockingOffset(0)
        , mLockingSize(0)
    {
    }

    DiGLVertexBuffer::~DiGLVertexBuffer()
    {
        Release();
    }

    void DiGLVertexBuffer::Create(uint32 size, uint32 usage, uint16 streamID)
    {
        if (mBufferId)
        {
            DI_WARNING("The GL vertex buffer id has already existed. It should be deleted before creating a new one.");
            glDeleteBuffersARB(1, &mBufferId);
        }

        mBufferSize = size;
        mResUsage = (DiResUsage)usage;

        glGenBuffersARB(1, &mBufferId);

        if (!mBufferId)
        {
            DI_WARNING("Cannot create OpenGL vertex buffer");
            return;
        }

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, mBufferId);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, mBufferSize, NULL, DiGLTypeMappings::GetGLUsage(mResUsage));
    }

    void DiGLVertexBuffer::Bind()
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, mBufferId);
    }

    void DiGLVertexBuffer::Release()
    {
        glDeleteBuffersARB(1, &mBufferId);
    }

    void DiGLVertexBuffer::OnDeviceLost()
    {
    }

    void DiGLVertexBuffer::OnDeviceReset()
    {
    }

    void* DiGLVertexBuffer::Lock(uint32 offset, uint32 size, DiLockFlag flag /*= LOCK_NORMAL*/)
    {
        mLockingOffset = offset;
        mLockingSize = size;

        mLockingScratch = DiGLDriver::BufferMgr->AllocScratchBuffer(size);
        return mLockingScratch;
    }

    void DiGLVertexBuffer::Unlock()
    {
        SetDataRange(mLockingScratch, mLockingOffset, mLockingSize);
        DiGLDriver::BufferMgr->DeallocScratchBuffer(mLockingScratch);
        mLockingScratch = nullptr;
        mLockingOffset = 0;
        mLockingSize = 0;
    }

    bool DiGLVertexBuffer::SetData(const void* data)
    {
        if (!data)
            return false;

        glBindBufferARB(GL_ARRAY_BUFFER, mBufferId);
        glBufferDataARB(GL_ARRAY_BUFFER, mBufferSize, data, DiGLTypeMappings::GetGLUsage(mResUsage));

        return true;
    }

    bool DiGLVertexBuffer::SetDataRange(const void* data, uint32 start,
        uint32 count, bool discard /*= false*/)
    {
        if (start == 0 && count == mLockingSize)
            return SetData(data);

        if (!data || !count)
            return false;

        if (start + count > mBufferSize)
        {
            DI_WARNING("Invalid vertex buffer data range");
            return false;
        }

        glBindBufferARB(GL_ARRAY_BUFFER, mBufferId);

        if (!discard || start != 0)
            glBufferSubDataARB(GL_ARRAY_BUFFER, start, count, data);
        else
            glBufferDataARB(GL_ARRAY_BUFFER, count, data, DiGLTypeMappings::GetGLUsage(mResUsage));

        return true;
    }

}