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

#ifndef PathLib_h__
#define PathLib_h__

#include "MiscPrerequisites.h"

namespace Demi
{
    class DI_MISC_API DiPathLib
    {
    public:
        
        /** For example "C:\Demi\Bin\Debug\Sample_demi.exe"
         */
        static const DiString& GetApplicationFileName();

        /** For example "C:\Demi\Bin\Debug\"
         */
        static const DiString& GetApplicationPath();

        /** we need a absolute path
        */
        static bool FileExisted(const DiString& file);

        static DiString        msAppFile;
    };
}

#endif // PathLib_h__