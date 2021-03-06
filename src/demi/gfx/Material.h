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

#ifndef DiMaterial_h__
#define DiMaterial_h__


#include "GfxPrerequisites.h"
#include "GpuProgram.h"
#include "Asset.h"
#include "ShaderProgram.h"

namespace Demi 
{
    class DI_GFX_API DiMaterial : public DiAsset
    {
    public:

        friend class                DiShaderParameter;
        friend class                DiAssetManager;
        friend class                DiRenderPipeline;

    public:

        DiMaterial(const DiString& name);

        virtual                     ~DiMaterial(void);

    public:

        void                        Release() { delete this; }

        DiCullMode                  GetCullMode() const { return mCullMode; }

        void                        SetCullMode(DiCullMode val);

        const DiString&             GetAssetType() const  { return TYPE; }

        bool                        GetDepthWrite() const { return mDepthWrite; }
                                    
        void                        SetDepthWrite(bool val) { mDepthWrite = val; }
                                    
        bool                        GetDepthCheck() const { return mDepthCheck; }
                                    
        void                        SetDepthCheck(bool val) { mDepthCheck = val; }
                                    
        DiShaderParameter*          GetShaderParameter() const { return mShaderParameter; }

        bool                        HasTexture();

        void                        LoadShader(const DiString& vsname,const DiString& psname);

        DiString                    GetVertexShaderName() const;

        DiString                    GetPixelShaderName() const;

        DiShaderProgram*            GetVertexShader(){return mVertexShader;}

        DiShaderProgram*            GetPixelShader(){return mPixelShader;}
        
        bool                        Load(DiDataStreamPtr data);

        bool                        Load();

        bool                        LoadingComplete() const;

        bool                        IsWireframe() const { return mWireframe; }

        void                        SetWireframe(bool val) { mWireframe = val; }

        bool                        GetForceWireframe() const { return mForceWireframe; }

        void                        SetForceWireframe(bool val) { mForceWireframe = val; }

        DiBlendMode                 GetBlendMode() const { return mBlendMode; }

        void                        SetBlendMode(DiBlendMode val) { mBlendMode = val; }

        bool                        IsTransparent() const;

        void                        SetInstanceType(DiInstanceType val);

        DiInstanceType              GetEnableInstance() const {return mInstanceState;}

        void                        Bind() const;

        void                        SetShaderFlags(uint64 f){mShaderFlag = f;}

        uint64                      GetShaderFlags()const{return mShaderFlag;}

        void                        SetShaderFlag(uint64 f){mShaderFlag |= f;}

        void                        ClearShaderFlag(uint64 f){mShaderFlag &= (~f);}

        void                        BindVariable( const DiGpuVariable &variable, 
                                        const void *data) const;

        void                        RecompileShader(DiShaderType shader,
                                        const DiPair<DiString,DiString>& marco);

        DiMaterialPtr               Clone(const DiString& newname);

        DiMaterialPtr               Clone();

        static DiMaterialPtr        QuickCreate(const DiString& vs, const DiString& ps, uint64 flag = 0);
                                    
        static DiMaterialPtr        QuickCreate(const DiString& name, const DiString& vs, const DiString& ps, uint64 flag = 0);

        // common parameters

        DiColor                     GetDiffuse() const { return mDiffuse; }

        void                        SetDiffuse(DiColor val) { mDiffuse = val; }

        DiColor                     GetAmbient() const { return mAmbient; }

        void                        SetAmbient(DiColor val) { mAmbient = val; }

        float                       GetOpacity() const { return mOpacity; }

        void                        SetOpacity(float val) { mOpacity = val; }

        float                       GetShininess() const { return mShininess; }

        void                        SetShininess(float val) { mShininess = val; }

        DiColor                     GetSpecular() const { return mSpecular; }
        
        void                        SetSpecular(DiColor val) { mSpecular = val; }

    public:

        /** This static property is needed for assets creating method
         */
        static DiString             TYPE;
        
        static DiMaterialPtr        GetShadowCasterMaterial(uint64 shaderflag);

    protected:

        DiCullMode                  mCullMode;

        bool                        mDepthWrite;

        bool                        mDepthCheck;

        bool                        mWireframe;

        bool                        mForceWireframe;

        DiInstanceType              mInstanceState;

        DiBlendMode                 mBlendMode;
        
        DiString                    mVSName;
        
        DiString                    mPSName;
        
        DiShaderParameter*          mShaderParameter{ nullptr };

        DiShaderProgram*            mVertexShader{ nullptr };
        
        DiShaderProgram*            mPixelShader{ nullptr };

        float                       mOpacity;

        float                       mShininess;

        DiColor                     mAmbient;

        DiColor                     mDiffuse;
        
        DiColor                     mSpecular;

        uint64                      mShaderFlag;
        
        static DiMap<uint64, DiMaterialPtr> sShadowCasterMats;
    };
}

#endif
