#include "stdafx.h"
#include "WaveMaterial.h"
#include "Graphics/CameraDepthRenderer.h"

WaveMaterial::WaveMaterial() : Material(L"Effects/Depth/WaveCartoonShader.fx") {}

void WaveMaterial::InitializeEffectVariables()
{
}

void WaveMaterial::SetNoiseTexture(const std::wstring& assetFile)
{
	//set noise texture
	m_pNoiseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gSurfaceNoise", m_pNoiseTexture);
}

void WaveMaterial::SetDistortTexture(const std::wstring& assetFile)
{
	//set distort texture
	m_pDistortTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gSurfaceDistortion", m_pDistortTexture);
}

void WaveMaterial::Update(float elapsedTime)
{
	//update time
	m_Time += elapsedTime * m_Speed;
	if (m_Time >= 10.f) m_Time = 0.f;
	SetVariable_Scalar(L"gTime", m_Time);
}

void WaveMaterial::OnUpdateModelVariables(const SceneContext& /*sceneContext*/, const ModelComponent* /*pModel*/) const
{
	const auto pDepthMapRenderer = CameraDepthRenderer::Get();
	
	//Update the CameraDepthMap texture
	SetVariable_Texture(L"gCameraDepthTexture", pDepthMapRenderer->GetDepthMap());
}
