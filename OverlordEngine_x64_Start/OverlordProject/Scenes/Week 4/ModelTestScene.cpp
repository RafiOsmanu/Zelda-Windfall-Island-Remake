#include "stdafx.h"
#include "ModelTestScene.h"
#include "Content/PxMeshLoader.h"
#include "Materials/ColorMaterial.h"
#include "Managers/MaterialManager.h"
#include "Components/ModelComponent.h"
#include "Materials/DiffuseMaterial.h"



ModelTestScene::ModelTestScene() :
	GameScene(L"ModelTestScene") {}


void ModelTestScene::Initialize()
{
	auto& physX = PxGetPhysics();
	auto pGroundMaterial = physX.createMaterial(.5f, .5f, .5f);

	//Ground Plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pGroundMaterial);


	//convex Chair
	m_pConvexChair = new GameObject();
	AddChild(m_pConvexChair);

	DiffuseMaterial* pChairColor = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pChairColor->SetDiffuseTexture(L"Textures/Chair_Dark.dds");

	

	const auto pModel = new ModelComponent(L"Meshes/Chair.ovm");
	pModel->SetMaterial(pChairColor);

	m_pConvexChair->AddComponent(pModel);

	const auto pChairConvex = ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc");

	auto pRigidBody = m_pConvexChair->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxConvexMeshGeometry{ pChairConvex }, *pGroundMaterial);
	pRigidBody->Translate(XMFLOAT3(0.f, 3.f, 0.f));
	
	
}

void ModelTestScene::Update()
{
}

void ModelTestScene::Draw()
{
}

void ModelTestScene::OnGUI()
{
}
