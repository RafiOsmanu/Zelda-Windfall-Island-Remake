#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial() : Material(L"Effects/UberShader.fx") {}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
    // Load the texture using the content manager
    m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

    // Assign the texture to the effect variable
    BaseMaterial::SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
    BaseMaterial::SetVariable_Scalar (L"gUseTextureDiffuse", true);
    InitializeEffectVariables();
    
}

void UberMaterial::InitializeEffectVariables()
{
    //m_pSpecularTexture = ContentManager::Load<TextureData>(L"Textures/Skulls_Heightmap.tga");
    //SetVariable_Texture(L"gTextureSpecularIntensity", m_pSpecularTexture);
    //SetVariable_Scalar(L"gUseTextureSpecularIntensity", true);
    //SetVariable_Vector(L"gColorDiffuse", XMFLOAT3(1.f, 0.f, 0.f));

   // m_pNormalTexture = ContentManager::Load<TextureData>(L"Textures/Skulls_Normalmap.tga");
    //SetVariable_Texture(L"gTextureNormal", m_pNormalTexture);
    //SetVariable_Scalar(L"gUseTextureNormal", true);
    //SetVariable_Scalar(L"gShininess", 40);

    //m_pEnviorementMap = ContentManager::Load<TextureData>(L"Textures/Default_reflection.dds");
   // SetVariable_Texture(L"gCubeEnvironment", m_pEnviorementMap);
    //SetVariable_Scalar(L"gUseEnvironmentMapping", true);
    //SetVariable_Scalar(L"gReflectionStrength", 0.8f);
    //SetVariable_Scalar(L"gRefractionStrength", 0.5f);
    //SetVariable_Scalar(L"gRefractionIndex", 0.9f);


   //SetVariable_Scalar(L"gUseFresnelFalloff", true); 
   // SetVariable_Vector(L"gColorFresnel", XMFLOAT3(1.f, 1.f, 1.f));


    
   
}
