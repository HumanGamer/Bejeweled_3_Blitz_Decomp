#pragma once

#include "Common.h"

namespace Sexy
{
    class RenderEffectDefinition
    {
    public:
        struct Annotation
        {
            enum EAnnotType
            {
                AT_Bool,
                AT_Int,
                AT_Float,
                AT_Vector,
                AT_String,
            };

            EAnnotType mType;
            std::vector<unsigned char> mData;
            std::string mName;
        };

        struct StateCommand
        {
            enum ECommandType
            {
                CMD_None,
                CMD_SetRenderState,
                CMD_SetSamplerState,
                CMD_SetTextureStageState,
            };

            ECommandType mType;
            int mState;
            int mSamplerOrTextureStage;
            int mValue;
        };

        struct ShaderConstant
        {
            enum EConstantType
            {
                CT_Float,
                CT_Sampler,
            };

            enum EStandardConstantSemantic
            {
                SCS_None = 0x0,
                SCS_World = 0x1,
                SCS_View = 0x2,
                SCS_Proj = 0x4,
                SCS_Inv = 0x8,
                SCS_Texture = 0x10,
                SCS_LightAmbient = 0x20,
                SCS_LightAttenuation = 0x21,
                SCS_LightDiffuse = 0x22,
                SCS_LightSpecular = 0x23,
                SCS_LightDirection = 0x24,
                SCS_LightPosition = 0x25,
                SCS_LightMisc = 0x26,
                SCS_MaterialAmbient = 0x27,
                SCS_MaterialDiffuse = 0x28,
                SCS_MaterialSpecular = 0x29,
                SCS_MaterialEmissive = 0x2A,
                SCS_MaterialPower = 0x2B,
                SCS_GlobalAmbient = 0x2C,
                SCS_TextureFactor = 0x2D,
                SCS_LIGHTFIRST = 0x20,
                SCS_LIGHTLAST = 0x26,
                SCS_MATERIALFIRST = 0x27,
                SCS_MATERIALLAST = 0x2B,
            };

            EConstantType mType;
            std::string mConstantName;
            std::string mSemantic;
            EStandardConstantSemantic mStandardSemantic;
            int mRegisterIndex;
            int mRegisterCount;
        };

        struct Shader
        {
            std::vector<unsigned char> mCode;
            std::vector<ShaderConstant> mConstants;
        };

        struct Pass
        {
            std::string mPassName;
            std::vector<Annotation> mAnnotations;
            std::vector<StateCommand> mStateCommands;
            Shader mVertexShader;
            Shader mPixelShader;
        };

        struct Technique
        {
            std::string mTechniqueName;
            std::vector<Annotation> mAnnotations;
            std::vector<Pass> mPasses;
        };

        std::vector<Technique> mTechniques;
        std::string mSrcFileName;
    };
}
