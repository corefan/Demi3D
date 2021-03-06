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

#ifndef DiK2Plugin_h__
#define DiK2Plugin_h__

#include "K2Prerequisites.h"
#include "EnginePlugin.h"

namespace Demi
{
    class DEMI_K2_API DiK2Plugin : public DiPlugin
    {
    public:

        DiK2Plugin(){}

        ~DiK2Plugin(){}

    public:

        const DiString& GetName() const;

        DI_PLUGIN void  Install();

        DI_PLUGIN void  Uninstall();
    };
}

#endif