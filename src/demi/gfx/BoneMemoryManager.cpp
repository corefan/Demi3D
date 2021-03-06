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

/// This file is adapted from Ogre 2.0 (unstable version)

#include "GfxPch.h"
#include "BoneMemoryManager.h"
#include "NewBone.h"
#include "SkeletonAnimManager.h"

namespace Demi
{
    BoneMemoryManager::BoneMemoryManager() : mBoneRebaseListener( 0 )
    {
    }
    
    BoneMemoryManager::~BoneMemoryManager()
    {
        auto itor = mMemoryManagers.begin();
        auto end  = mMemoryManagers.end();

        while( itor != end )
        {
            itor->destroy();
            ++itor;
        }

        mMemoryManagers.clear();
    }
    
    void BoneMemoryManager::growToDepth( size_t newDepth )
    {
        //TODO: (dark_sylinc) give a specialized hint for each depth.
        while( newDepth >= mMemoryManagers.size() )
        {
            //Disable cleanups if we fall here (use _growToDepth instead)
            mMemoryManagers.push_back( BoneArrayMemoryManager( mMemoryManagers.size(), 100,
                                                                -1, ArrayMemoryManager::MAX_MEMORY_SLOTS,
                                                                this ) );
            mMemoryManagers.back().initialize();
        }
    }
    
    void BoneMemoryManager::_growToDepth( const DiVector<size_t> &bonesPerDepth )
    {
        mMemoryManagers.reserve( bonesPerDepth.size() );
        //TODO: (dark_sylinc) give a specialized hint for each depth (number of skeletons).
        while( bonesPerDepth.size() > mMemoryManagers.size() )
        {
            size_t depthIdx = mMemoryManagers.size();

            size_t numSlots = bonesPerDepth[depthIdx];
            if( numSlots > ARRAY_PACKED_REALS >> 1 )
            {
                //For bones, cleanup threshold must be multiple of ARRAY_PACKED_REALS - 1.
                numSlots = (((bonesPerDepth[depthIdx] - 1) / ARRAY_PACKED_REALS) + 1) *
                                                                    ARRAY_PACKED_REALS;
            }

            size_t cleanupThreshold = 50 * numSlots - 1; //50 is arbitrary
            mMemoryManagers.push_back( BoneArrayMemoryManager( mMemoryManagers.size(), 100,
                                                                cleanupThreshold,
                                                                ArrayMemoryManager::MAX_MEMORY_SLOTS,
                                                                this ) );
            mMemoryManagers.back().initialize();
        }
    }
    
    void BoneMemoryManager::nodeCreated( BoneTransform &outTransform, size_t depth )
    {
        growToDepth( depth );

        BoneArrayMemoryManager& mgr = mMemoryManagers[depth];
        mgr.createNewNode( outTransform );
    }
    
    void BoneMemoryManager::nodeAttached( BoneTransform &outTransform, size_t depth )
    {
        growToDepth( depth );

        BoneTransform tmp;
        mMemoryManagers[depth].createNewNode( tmp );

        tmp.copy( outTransform );

        BoneArrayMemoryManager &mgr = mMemoryManagers[0];
        mgr.destroyNode( outTransform );

        outTransform = tmp;
    }
    
    void BoneMemoryManager::nodeDettached( BoneTransform &outTransform, size_t depth )
    {
        BoneTransform tmp;
        mMemoryManagers[0].createNewNode( tmp );

        tmp.copy( outTransform );

        BoneArrayMemoryManager &mgr = mMemoryManagers[depth];
        mgr.destroyNode( outTransform );

        outTransform = tmp;
    }
    
    void BoneMemoryManager::nodeDestroyed( BoneTransform &outTransform, size_t depth )
    {
        BoneArrayMemoryManager &mgr = mMemoryManagers[depth];
        mgr.destroyNode( outTransform );
    }
    
    void BoneMemoryManager::nodeMoved( BoneTransform &inOutTransform, size_t oldDepth, size_t newDepth )
    {
        growToDepth( newDepth );

        BoneTransform tmp;
        mMemoryManagers[newDepth].createNewNode( tmp );

        tmp.copy( inOutTransform );

        BoneArrayMemoryManager &mgr = mMemoryManagers[oldDepth];
        mgr.destroyNode( inOutTransform );

        inOutTransform = tmp;
    }
    
    void BoneMemoryManager::migrateTo( BoneTransform &inOutTransform, size_t depth,
                                        BoneMemoryManager *dstBoneMemoryManager )
    {
        BoneTransform tmp;
        dstBoneMemoryManager->nodeCreated( tmp, depth );
        tmp.copy( inOutTransform );
        this->nodeDestroyed( inOutTransform, depth );
        inOutTransform = tmp;
    }
    
    size_t BoneMemoryManager::getNumDepths() const
    {
        size_t retVal = -1;
        ArrayMemoryManagerVec::const_iterator begin= mMemoryManagers.begin();
        ArrayMemoryManagerVec::const_iterator itor = mMemoryManagers.begin();
        ArrayMemoryManagerVec::const_iterator end  = mMemoryManagers.end();

        while( itor != end )
        {
            if( itor->getUsedMemory() )
                retVal = itor - begin;
            ++itor;
        }

        return retVal + 1;
    }
    
    size_t BoneMemoryManager::getFirstNode( BoneTransform &outTransform, size_t depth )
    {
        return mMemoryManagers[depth].getFirstNode( outTransform );
    }
    
    void BoneMemoryManager::buildDiffList( ArrayMemoryManager::ManagerType managerType, uint16 level,
                                            const MemoryPoolVec &basePtrs,
                                            ArrayMemoryManager::PtrdiffVec &outDiffsList )
    {
        //We don't need to build the diff list as we've access to the Node through mOwner
        //and access to the actual Node with the right pointers.
        /*Transform transform;
        const size_t numNodes = this->getFirstNode( transform, level );

        for( size_t i=0; i<numNodes; i += ARRAY_PACKED_REALS )
        {
            for( size_t j=0; j<ARRAY_PACKED_REALS; ++j )
            {
                if( transform.mOwner[j] )
                {
                    outDiffsList.push_back( (transform.mParents + transform.mIndex) -
                                        (Ogre::Node**)basePtrs[BoneArrayMemoryManager::Parent] );
                }
            }
            transform.advancePack();
        }*/
    }
    //---------------------------------------------------------------------
    void BoneMemoryManager::applyRebase( ArrayMemoryManager::ManagerType managerType, uint16 level,
                                            const MemoryPoolVec &newBasePtrs,
                                            const ArrayMemoryManager::PtrdiffVec &diffsList )
    {
        BoneTransform transform;
        const size_t numNodes = this->getFirstNode( transform, level );

        for( size_t i=0; i<numNodes; i += ARRAY_PACKED_REALS )
        {
            for( size_t j=0; j<ARRAY_PACKED_REALS; ++j )
            {
                if( transform.mOwner[j] )
                {
                    transform.mIndex = j;
                    transform.mOwner[j]->_getTransform() = transform;
                    transform.mOwner[j]->_memoryRebased();
                }
            }

            transform.advancePack();
        }

        if( mBoneRebaseListener )
            mBoneRebaseListener->_updateBoneStartTransforms();
    }
    //---------------------------------------------------------------------
    void BoneMemoryManager::performCleanup( ArrayMemoryManager::ManagerType managerType, uint16 level,
                                        const MemoryPoolVec &basePtrs, size_t const *elementsMemSizes,
                                        size_t startInstance, size_t diffInstances )
    {
        BoneTransform transform;
        const size_t numNodes = this->getFirstNode( transform, level );

        size_t roundedStart = startInstance / ARRAY_PACKED_REALS;

        transform.advancePack( roundedStart );

        for( size_t i=roundedStart * ARRAY_PACKED_REALS; i<numNodes; i += ARRAY_PACKED_REALS )
        {
            for (size_t j = 0; j < ARRAY_PACKED_REALS; ++j)
            {
                if( transform.mOwner[j] )
                {
                    transform.mIndex = j;
                    transform.mOwner[j]->_getTransform() = transform;
                    transform.mOwner[j]->_memoryRebased();
                }
            }

            transform.advancePack();
        }

        if( mBoneRebaseListener )
            mBoneRebaseListener->_updateBoneStartTransforms();
    }
}
