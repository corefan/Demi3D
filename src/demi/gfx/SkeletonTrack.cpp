
/**********************************************************************
This source file is a part of Demi3D
   __  ___  __  __  __
  |  \|_ |\/||   _)|  \ 
  |__/|__|  ||  __)|__/ 

Copyright (c) 2013-2014 Demi team
https://github.com/wangyanxing/Demi3D

Released under the MIT License
https://github.com/wangyanxing/Demi3D/blob/master/License.txt
***********************************************************************/

#include "GfxPch.h"

#include "SkeletonTrack.h"
#include "Math/Array/Mathlib.h"
#include "Math/Array/BoneTransform.h"
#include "KfTransformArrayMemoryManager.h"

namespace Demi
{
    SkeletonTrack::SkeletonTrack( uint32 boneBlockIdx,
                                    KfTransformArrayMemoryManager *kfTransformMemoryManager ) :
        mKeyFrameRigs( 0 ),
        mNumFrames( 0 ),
        mBoneBlockIdx( boneBlockIdx ),
        mUsedSlots( 0 ),
        mLocalMemoryManager( kfTransformMemoryManager )
    {
    }
    
    SkeletonTrack::~SkeletonTrack()
    {
    }
    
    void SkeletonTrack::setNumKeyFrame( size_t numKeyFrames )
    {
        mKeyFrameRigs.reserve( numKeyFrames );
    }
    
    void SkeletonTrack::addKeyFrame( float timestamp, float frameRate )
    {
        assert( mKeyFrameRigs.empty() || timestamp > mKeyFrameRigs.back().mFrame );

        mKeyFrameRigs.push_back( KeyFrameRig() );
        KeyFrameRig &keyFrame = mKeyFrameRigs.back();
        keyFrame.mFrame = timestamp * frameRate;
        keyFrame.mInvNextFrameDistance = 1.0f;
        if( mKeyFrameRigs.size() > 1 )
        {
            KeyFrameRig &prevKeyFrame = mKeyFrameRigs[mKeyFrameRigs.size()-2];
            prevKeyFrame.mInvNextFrameDistance = 1.0f / (keyFrame.mFrame - prevKeyFrame.mFrame);
        }

        mLocalMemoryManager->createNewNode( (KfTransform**)(&keyFrame.mBoneTransform) );
    }
    
    void SkeletonTrack::setKeyFrameTransform( float frame, uint32 slot, const DiVec3 &vPos,
                                                const DiQuat &qRot, const DiVec3 vScale )
    {
        KeyFrameRigVec::iterator itor = mKeyFrameRigs.begin();
        KeyFrameRigVec::iterator end  = mKeyFrameRigs.end();

        while( itor != end && DiMath::Abs( itor->mFrame - frame ) < 1e-6f )
            ++itor;

        if( itor == mKeyFrameRigs.end() )
        {
            DI_WARNING("Frame not found" );
        }

        itor->mBoneTransform->mPosition.setFromVector3( vPos, slot );
        itor->mBoneTransform->mOrientation.setFromQuaternion( qRot, slot );
        itor->mBoneTransform->mScale.setFromVector3( vScale, slot );

        mUsedSlots = std::max( slot+1, mUsedSlots );
    }
    
    inline void SkeletonTrack::getKeyFrameRigAt( KeyFrameRigVec::const_iterator &inOutPrevFrame,
                                                    KeyFrameRigVec::const_iterator &outNextFrame,
                                                    float frame ) const
    {
        KeyFrameRigVec::const_iterator prevFrame = inOutPrevFrame;
        KeyFrameRigVec::const_iterator nextFrame = inOutPrevFrame;

        if( frame >= nextFrame->mFrame )
        {
            while( nextFrame != (mKeyFrameRigs.end() - 1) && nextFrame->mFrame <= frame )
                prevFrame = nextFrame++;
        }
        else
        {
            while( prevFrame != mKeyFrameRigs.begin() && prevFrame->mFrame > frame )
                nextFrame = prevFrame--;
        }

        inOutPrevFrame  = prevFrame;
        outNextFrame    = nextFrame;
    }
    
    void SkeletonTrack::applyKeyFrameRigAt( KeyFrameRigVec::const_iterator &inOutLastKnownKeyFrameRig,
                                            float frame, ArrayFloat animWeight,
                                            const ArrayFloat * RESTRICT_ALIAS perBoneWeights,
                                            const TransformArray &boneTransforms ) const
    {
        KeyFrameRigVec::const_iterator prevFrame = inOutLastKnownKeyFrameRig;
        KeyFrameRigVec::const_iterator nextFrame;
        getKeyFrameRigAt( prevFrame, nextFrame, frame );

        const float scalarW = (frame - prevFrame->mFrame) * prevFrame->mInvNextFrameDistance;
        ArrayFloat fTimeW = Mathlib::SetAll( scalarW );

        size_t level    = mBoneBlockIdx >> 24;
        size_t offset   = mBoneBlockIdx & 0x00FFFFFF;

        ArrayVector3 * RESTRICT_ALIAS finalPos      = boneTransforms[level].mPosition + offset;
        ArrayVector3 * RESTRICT_ALIAS finalScale    = boneTransforms[level].mScale + offset;
        ArrayQuaternion * RESTRICT_ALIAS finalRot   = boneTransforms[level].mOrientation + offset;

        KfTransform * RESTRICT_ALIAS prevTransf = prevFrame->mBoneTransform;
        KfTransform * RESTRICT_ALIAS nextTransf = nextFrame->mBoneTransform;

        ArrayVector3 interpPos, interpScale;
        ArrayQuaternion interpRot;
        //Interpolate keyframes' rotation not using shortestPath to respect the original animation
        interpPos   = DiMath::Lerp( prevTransf->mPosition, nextTransf->mPosition, fTimeW );
        interpRot   = ArrayQuaternion::nlerp( fTimeW, prevTransf->mOrientation,
                                                        nextTransf->mOrientation );
        interpScale = DiMath::Lerp( prevTransf->mScale, nextTransf->mScale, fTimeW );

        //Combine our internal flag (that prevents blending
        //unanimated bones) with user's custom weights
        ArrayFloat fW = (*perBoneWeights) * animWeight;

        //When mixing, also interpolate rotation not using shortest path; as this is usually desired
        *finalPos   += interpPos * fW;
        *finalScale *= DiMath::Lerp( ArrayVector3::UNIT_SCALE, interpScale, fW );
        *finalRot   = ArrayQuaternion::nlerp( fW, ArrayQuaternion::IDENTITY, interpRot ) *
                        (*finalRot);

        inOutLastKnownKeyFrameRig = prevFrame;
    }
    
    void SkeletonTrack::_bakeUnusedSlots(void)
    {
        DI_ASSERT( mUsedSlots <= ARRAY_PACKED_REALS );

        if( mUsedSlots <= (ARRAY_PACKED_REALS >> 1) )
        {
            KeyFrameRigVec::const_iterator itor = mKeyFrameRigs.begin();
            KeyFrameRigVec::const_iterator end  = mKeyFrameRigs.end();

            while( itor != end )
            {
                size_t j=0;
                for( size_t i=mUsedSlots; i<ARRAY_PACKED_REALS; ++i )
                {
                    DiVec3 vTmp;
                    DiQuat qTmp;
                    itor->mBoneTransform->mPosition.getAsVector3( vTmp, j );
                    itor->mBoneTransform->mPosition.setFromVector3( vTmp, i );
                    itor->mBoneTransform->mOrientation.getAsQuaternion( qTmp, j );
                    itor->mBoneTransform->mOrientation.setFromQuaternion( qTmp, i );
                    itor->mBoneTransform->mScale.getAsVector3( vTmp, j );
                    itor->mBoneTransform->mScale.setFromVector3( vTmp, i );

                    j = (j+1) % mUsedSlots;
                }
                ++itor;
            }
        }
    }
}
