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

#ifndef DiDirectionalLight_h__
#define DiDirectionalLight_h__


#include "light.h"

namespace Demi
{
    /** Directional light
        only contains direction and color
     */
    class DI_GFX_API DiDirLight : public DiLight
    {
    public:

        DiDirLight();

        virtual         ~DiDirLight();

    public:

        void            Update(DiCamera*);

        void            SetDirection(const DiVec3& vec);

        DiVec3          GetDirection() const {return mDirection;}

        const DiAABB&   GetBoundingBox(void) const;

        void            SetupShadowCamera(DiSceneManager* sceneManager);

        void            GetShadowCameraForCascade(const DiCamera *cam, DiCamera *texCam,
                            uint16 iteration, float nearSplit, float farSplit);

    protected:

        void            SetupShadowCameraImpl(DiSceneManager* sceneManager, DiCamera* shadowCamera,
                                             float nearSplit, float farSplit);

        void            QuantizeShadowCamera(DiCamera* shadowCamera, const DiRect& shadowViewport, const DiAABB& viewBox);

    protected:

        DiVec3          mDirection;
        
    };
}

#endif
