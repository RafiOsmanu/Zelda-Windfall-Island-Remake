#include "stdafx.h"
#include "UberMaterialScene.h"
#include "Content/PxMeshLoader.h"
#include "Materials/ColorMaterial.h"
#include "Managers/MaterialManager.h"
#include "Components/ModelComponent.h"


UberMaterialScene::UberMaterialScene() : GameScene(L"UberMaterialScene") {}

void UberMaterialScene::Initialize()
{

	m_pSphere = new GameObject();
	m_pSphere->GetTransform()->Scale(10.f);
	AddChild(m_pSphere);

	m_pSphereMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	m_pSphereMaterial->SetDiffuseTexture(L"Textures/Skulls_Diffusemap.tga");


	const auto pModel = new ModelComponent(L"Meshes/Sphere.ovm");
	pModel->SetMaterial(m_pSphereMaterial);
	
	
	

	m_pSphere->AddComponent(pModel);

	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;
	
	
	
}

void UberMaterialScene::Update()
{
	
}

void UberMaterialScene::Draw()
{
}

void UberMaterialScene::OnGUI()
{
	m_pSphereMaterial->DrawImGui();
}
