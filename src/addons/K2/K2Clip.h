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

#ifndef DiK2Clip_h__
#define DiK2Clip_h__

#include "K2Prerequisites.h"
#include "K2GameDefines.h"

namespace Demi
{
    /** It is used for binding pose and bone hierarchy
        just data
     */
    class DiK2BonesData
    {
    public:
        DiVector<DiString>   names;
        DiVector<DiMat4>     trans;
        DiVector<DiMat4>     invtrans;
        DiVector<int>        parents;
        DiMap<DiString, int> nameMap;
    };

    /** A k2 skeleton instance
     */
    class DEMI_K2_API DiK2Skeleton
    {
    public:

        DiK2Skeleton();

        ~DiK2Skeleton();

    public:

        void            Destroy();

        /** Create the whole skeleton hierarchy
        */
        void            CreateBones(DiK2BonesDataPtr boneData);

        DiNode*         GetRootBone() { return mRootNode; }

        uint32          GetNumBones() const { return (uint32)mBones.size(); }

        DiMat4*         GetBoneMatrices() { return mBoneMatrices; }

        DiNode*         GetBone(uint32 id) { return mBones[id]; }

        bool            HasBone(const DiString& name) const { return mBoneNames.find(name) != mBoneNames.end(); }

        DiNode*         GetBone(const DiString& name) { return mBoneNames[name]; }

        /** Apply an animation clip
         */
        void            Apply(DiK2Animation* anim);

        void            CacheBoneMatrices();
        
    private:

        DiNode*             mRootNode;

        DiMat4*             mBoneMatrices;

        DiVector<DiNode*>   mBones;

        DiStrHash<DiNode*>  mBoneNames;

        DiK2BonesDataPtr    mRowData;
    };
    
    /** K2 animation clip
     */
    class DEMI_K2_API DiK2Clip
    {
    public:

        DiK2Clip();

        ~DiK2Clip();

        DiString    mName;

        bool        mLoop;

        float       mFPS;

        int         mCurFrame;
        
        /// interpolation factor [0,1]
        float       mInterpFactor;

        int         mNumFrames;

        /// current time
        float       mTime;

        void        Update(float deltaTime);
        
        /// for interpolation
        int         GetNextFrame()
        {
            return mCurFrame < mNumFrames-1 ? mCurFrame+1 : (mLoop ? 0 : mNumFrames - 1);
        }
        
        /// get the interpolated keyframe
        void        GetFrameTrans(const DiString& bone, Trans& output);
        
        void        Cleanup()
        {
            mCurFrame = 0;
            mInterpFactor = 0;
            mTime = 0;
        }

        K2KeyFrames* mKeyFrames;
    };

    /** k2 animation class
        very simple for now, nothing fancy
     */
    class DEMI_K2_API DiK2Animation
    {
    public:

        DiK2Animation();

        ~DiK2Animation();

    public:

        void        Play(const DiString& name);

        void        Play(DiK2Clip* clip);

        void        Play(K2PrefabClip::Clips clip);

        void        Update(float deltaTime);

        DiK2Clip*   AddClip(const DiString& name);

    public:

        /// source clip (for blending)
        DiK2Clip*         mSource;

        /// target clip (for blending)
        DiK2Clip*         mTarget;
        
        float             mBlendTime;
        
        float             mBlendElapsed;

        DiK2BonesData     mSkeleton;
        
        /// prefab clips
        DiK2Clip*         mClips[K2PrefabClip::MAX_PREFAB_ANIM];

        DiStrHash<DiK2Clip*> mExtraClips;
    };
}

#endif
