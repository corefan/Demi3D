
#include "GfxPch.h"
#include "Material.h"
#include "Memory.h"
#include "LogManager.h"
#include "ShaderParam.h"
#include "MathMisc.h"
#include "Texture.h"
#include "GfxDriver.h"
#include "MaterialSerial.h"
#include "AssetManager.h"
#include "MaterialPredefine.h"
#include "ShaderManager.h"
#include "GfxDriver.h"

namespace Demi 
{
    DiAssetType DiMaterial::TYPE = ASSET_MATERIAL;

    DiMaterial::DiMaterial(const DiString& name) :
        DiAsset(name),mShaderParameter(NULL),
        mAmbient(DiColor::Black)
    {
        mOpacity         = 1.0f;
        mDepthWrite      = true;
        mDepthCheck      = true;
        mCullMode        = CULL_CW;
        mWireframe       = false;
        mForceWireframe  = false;
        mBlendMode       = BLEND_REPLACE;
        mEnableVertColor = false;
        mInstanceState   = INSTANCE_DISABLE;
        mShaderFlag      = 0;
        mCommonVsType    = CVT_MAX_MATS;
        mVertexShader    = NULL;
        mPixelShader     = NULL;
        mShininess       = 32.0f;

        if (Driver)
            mShaderParameter = Driver->CreateShaderParams(*this);
        else
            mShaderParameter = nullptr;
    }

    DiMaterial::~DiMaterial(void)
    {
        SAFE_DELETE(mShaderParameter);
    }

    void DiMaterial::SetCullMode( DiCullMode val )
    {
        mCullMode = val;
    }

    void DiMaterial::Bind() const
    {
        if (!mVertexShader || !mPixelShader)
            return; // TODO: set a default material

        Driver->BindMaterialStates(this);

        DiShaderEnvironment &shaderEnv = *Driver->GetShaderEnvironment();

        // common lighting parameters
        shaderEnv.ambientColor   = mAmbient;
        shaderEnv.diffuseColor   = mDiffuse;
        shaderEnv.specularColor  = mSpecular;
        shaderEnv.opacity        = mOpacity;
        shaderEnv.shininess      = mShininess;

        Driver->BindShaders(mVertexShader, mPixelShader);
        mShaderParameter->Bind();
    }

    bool DiMaterial::Load( DiDataStreamPtr data )
    {
        DiMaterialSerializer ms;
        if(ms.ParseMaterial(data,this) == false)
        {
            DI_WARNING("Failed to load the material : %s",data->GetName().c_str());
            return false;
        }
        return true;
    }

    bool DiMaterial::Load()
    {
        return true;    
    }

    void DiMaterial::LoadShader(const DiString& vsname, const DiString& psname)
    {
        if (!DiShaderManager::GetInstancePtr())
            return;

        mVertexShader = DiShaderManager::GetInstance().LoadShader(vsname, SHADER_VERTEX, mShaderFlag);
        mPixelShader  = DiShaderManager::GetInstance().LoadShader(psname, SHADER_PIXEL , mShaderFlag);

        mShaderParameter->LoadParameters();
    }

    bool DiMaterial::LoadingComplete() const
    {
        return (mVertexShader && mPixelShader) ? true : false;
    }

    DiString DiMaterial::GetVertexShaderName() const
    {
        return mVertexShader ? mVertexShader->GetShaderFileName() : DiString::BLANK;
    }

    DiString DiMaterial::GetPixelShaderName() const
    {
        return mPixelShader ? mPixelShader->GetShaderFileName() : DiString::BLANK;
    }

    bool DiMaterial::HasTexture()
    {
        if (!mShaderParameter)
            return false;

        return mShaderParameter->HasVariableType(DiShaderParameter::VARIABLE_SAMPLER2D) ||
            mShaderParameter->HasVariableType(DiShaderParameter::VARIABLE_SAMPLERCUBE);
    }

    void DiMaterial::SetEnableVertColor( bool val )
    {
        if (mEnableVertColor != val)
            mEnableVertColor = val;
    }

    void DiMaterial::SetExtraParams(DiCompileDesc& desc)
    {
        desc.AddMarco(DiMaterialDefine::PARAM_VERTEX_COLOR,mEnableVertColor?"1":"0");
    }

    void DiMaterial::RecompileShader( DiShaderType type, const DiPair<DiString,DiString>& marco )
    {
        DiShaderProgram* shader = nullptr;

        if (type == SHADER_VERTEX)
            shader = mVertexShader;
        else if (type == SHADER_PIXEL)
            shader = mPixelShader;

        if (shader)
        {
            DiShaderInstance* si = shader->GetShader();
            si->mCompileDesc.AddMarco(marco.first,marco.second);
            si->Compile(shader->GetCode());
            mShaderParameter->LoadParameters();
        }
    }

    DiMaterialPtr DiMaterial::QuickCreate( const DiString& vs, const DiString& ps , uint64 flag)
    {
        static int quickcreateID = 0;
        DiString name;
        name.Format("quick_%d", quickcreateID++);
        DiMaterialPtr mat = DiAssetManager::GetInstance().CreateOrReplaceAsset<DiMaterial>(name);

        mat->SetShaderFlag(flag);
        mat->LoadShader(vs,ps);

        return mat;
    }

    DiMaterialPtr DiMaterial::QuickCreate(const DiString& name, const DiString& vs, const DiString& ps, uint64 flag )
    {
        DiMaterialPtr mat = DiAssetManager::GetInstance().CreateOrReplaceAsset<DiMaterial>(name);

        mat->SetShaderFlag(flag);
        mat->LoadShader(vs,ps);

        return mat;
    }

    DiMaterialPtr DiMaterial::Clone( const DiString& newname )
    {
        DiMaterialPtr mat = DiAssetManager::GetInstance().CreateManualAsset<DiMaterial>(newname);

        mat->LoadShader(mVertexShader->GetName(), mPixelShader->GetName());

        mat->mCullMode        = mCullMode;
        mat->mDepthWrite      = mDepthWrite;
        mat->mDepthCheck      = mDepthCheck;
        mat->mWireframe       = mWireframe;
        mat->mForceWireframe  = mForceWireframe;
        mat->mEnableVertColor = mEnableVertColor;
        mat->mBlendMode       = mBlendMode;

        mShaderParameter->CloneVarsTo(mat->GetShaderParameter());
        
        return mat;
    }

    void DiMaterial::SetInstanceType( DiInstanceType val )
    {
        if (mInstanceState != val)
            mInstanceState = val;
    }
}

bool DiMaterial::IsTransparent() const
{
    return mBlendMode == BLEND_ONE_INV_ALPHA || mBlendMode == BLEND_ALPHA;
}