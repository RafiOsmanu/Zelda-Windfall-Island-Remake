#include "stdafx.h"
#include "SkyBoxMaterial.h"

SkyBoxMaterial::SkyBoxMaterial() : Material(L"Effects/SkyBox.fx")
{
}


void SkyBoxMaterial::SetTexture(const std::wstring& assetFile)
{
	m_pSkyBoxTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"m_CubeMap", m_pSkyBoxTexture);
}

void SkyBoxMaterial::InitializeEffectVariables()
{
}
