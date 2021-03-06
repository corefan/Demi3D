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

#ifndef DiTerrainDesc_h__
#define DiTerrainDesc_h__

#include "K2Prerequisites.h"

namespace Demi
{
    struct K2TerrainTexture
    {
        uint32 ToHash() const
        {
            // pretty simple
            return (diffuse0 << 24) + (diffuse1 << 16) + (normal0 << 8) + normal1;
        }

        DEMI_K2_API friend bool operator <(const K2TerrainTexture& a, const K2TerrainTexture& b)
        {
            return a.ToHash() < b.ToHash();
        }

        uint16 diffuse0;
        uint16 normal0;

        uint16 diffuse1;
        uint16 normal1;
    };

    struct DEMI_K2_API DiFoliageLayerDesc
    {
        DiFoliageLayerDesc();

        void            Load(DiDataStreamPtr ds);

        void            Save(DiDataStreamPtr ds);

        void            ClearDensityMap();

        DiString        DemiureName;

        uint8           mRenderTechnique;

        float           mDensity;

        bool            mAnimate;

        bool            mLighting;

        float           mAnimMag;

        float           mAnimSpeed;

        float           mAnimFreq;

        float           mMinWidth, mMaxWidth;

        float           mMinHeight, mMaxHeight;

        ARGB            mColor;

        uint8           mDensityMap[FOLIAGE_DENSITY_MAP_SIZE * FOLIAGE_DENSITY_MAP_SIZE];
    };
    
    struct DiTerrainEntity 
    {
        DiModel*    pkModel;
        DiString    kName;    
    };
                
    class DEMI_K2_API DiTerrainDesc
    {
    public:

        DiTerrainDesc();

        ~DiTerrainDesc();

    public:

        bool                CheckValid() const;

        void                Release();

        uint32              GetVertNum();

        uint32              GetGridNum();
        
    public:

        float               mGridSize;    

        uint16              mSizeX;        

        uint16              mSizeY;        

        DiK2HeightMap*      mHeightMap;

        float               mTextureScale;

        DiK2VertexColorMap* mColorMap;

        uint32              mCliffSize; 

        DiK2TileCliffMap*   mTileCliffMap;

        DiK2VertexBlockerMap* mVertBlockerMap;

        uint8*              mWaterMap;

        float*              mWaterHeightMap;

        typedef DiMap<int,DiString> TextureTable;

        TextureTable        mTextureTable;

        DiK2TileMap*        mTextureIDMap;

        typedef DiVector<DiFoliageLayerDesc*> FoliageLayers;

        FoliageLayers       mFoliageLayers;
    };
}

#endif
