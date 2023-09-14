#include "stdafx.h"
#include "SkyBoxPrefab.h"

SkyBoxPrefab::SkyBoxPrefab(const std::wstring& textureName):
	m_pSkyBoxTextureName(textureName)
{
}

void SkyBoxPrefab::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pSkyBoxMaterial = MaterialManager::Get()->CreateMaterial<SkyBoxMaterial>();
	m_pSkyBoxMaterial->SetTexture(m_pSkyBoxTextureName);
	m_pSkyBoxModel = new ModelComponent(L"Meshes/Box.ovm");
	m_pSkyBoxModel->SetMaterial(m_pSkyBoxMaterial);
	AddComponent(m_pSkyBoxModel);
	
}
