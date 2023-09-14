#include "stdafx.h"
#include "DiffuseMaterial_Shadow.h"

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow.fx")
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	
	 //TODO_W8
	 //Update The Shader Variables
	 //Update the LightWVP > Used to Transform a vertex into Light clipping space using set variable matrix

	//Update Shadow Variables
	//(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	const auto pShadowMapRenderer = ShadowMapRenderer::Get();
	const auto lightWVP = XMMatrixMultiply(XMLoadFloat4x4(&pModel->GetTransform()->GetWorld()), XMLoadFloat4x4(&pShadowMapRenderer->GetLightVP()));

	SetVariable_Matrix(L"gWorldViewProj_Light", reinterpret_cast<const float*>(&lightWVP));

	 //
	 //2. Update the ShadowMap texture
	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());
	 //
	 //3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	
}
