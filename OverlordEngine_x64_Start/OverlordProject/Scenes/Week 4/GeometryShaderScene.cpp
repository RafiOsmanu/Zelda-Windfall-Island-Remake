#include "stdafx.h"
#include "GeometryShaderScene.h"
#include "Content/PxMeshLoader.h"
#include "Materials/ColorMaterial.h"
#include "Managers/MaterialManager.h"
#include "Components/ModelComponent.h"

GeometryShaderScene::GeometryShaderScene() : GameScene(L"GeometryShaderScene") {}

void GeometryShaderScene::Initialize()
{
	m_pSphere = new GameObject();
	m_pSphere->GetTransform()->Scale(5.f);
	AddChild(m_pSphere);

	m_pSphereMaterial = MaterialManager::Get()->CreateMaterial<GeometryMaterial>();
	m_pSphereMaterial->InitializeEffectVariables();

	const auto pModel = new ModelComponent(L"Meshes/octaSphere.ovm");
	pModel->SetMaterial(m_pSphereMaterial);

	m_pSphere->AddComponent(pModel);

	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;
}

void GeometryShaderScene::Update()
{
}

void GeometryShaderScene::Draw()
{
}

void GeometryShaderScene::OnGUI()
{
	m_pSphereMaterial->DrawImGui();
}
