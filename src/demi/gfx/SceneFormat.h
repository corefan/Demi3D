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

#ifndef DiSceneFormat_h__
#define DiSceneFormat_h__


namespace Demi
{
    enum DiSceneChunkID 
    {
        S_HEIGHT_MAP                        = 0x1000, 
        S_VERT_COLOR                        = 0x1100,
        S_LAYER_DATA                        = 0x1200,
        S_WATER_DATA                        = 0x1300,
        S_FOLIAGE_DATA                      = 0x1400,
        S_CLIFF_DATA                        = 0x1500,
    };
}

#endif
