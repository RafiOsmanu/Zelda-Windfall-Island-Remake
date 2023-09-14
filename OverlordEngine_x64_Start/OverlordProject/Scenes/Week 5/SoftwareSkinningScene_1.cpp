#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"
#include "Content/PxMeshLoader.h"
#include "Materials/ColorMaterial.h"
#include "Managers/MaterialManager.h"
#include "Components/ModelComponent.h"
#include "Prefabs/BoneObject.h"

SoftwareSkinningScene_1::SoftwareSkinningScene_1() : GameScene(L"SoftwareSkinningScene_1") {}

void SoftwareSkinningScene_1::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	//create material that is used for both bones
	const auto boneMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();

	//create a root object 
	GameObject* pRoot = new GameObject();
	AddChild(pRoot);

	//Initialize the first bone, This is then added to the pRoot as a child
	m_pBone0 = new BoneObject{ boneMaterial, 15.f };
	pRoot->AddChild(m_pBone0);

	//Initialize the second bone, This is then added to m_pBone0 as a child using add bone function
	m_pBone1 = new BoneObject{ boneMaterial, 15.f };
	m_pBone0->AddBone(m_pBone1);


}

void SoftwareSkinningScene_1::Update()
{
	if (m_IsAutoRotateOn)
	{
		m_Bone0Rotation.z += (m_RotationSign * 45.f * m_SceneContext.pGameTime->GetElapsed());
		m_Bone1Rotation.z += (m_RotationSign * 45.f * m_SceneContext.pGameTime->GetElapsed());

		if (m_Bone0Rotation.z > 45.f || m_Bone1Rotation.z > 45.f)
		{
			m_RotationSign = -1;
		}
		else if (m_Bone0Rotation.z < -45.f || m_Bone1Rotation.z < -45.f)
		{
			m_RotationSign = 1;
		}

	}
	m_pBone0->GetTransform()->Rotate(m_Bone0Rotation.x, m_Bone0Rotation.y, m_Bone0Rotation.z);
	m_pBone1->GetTransform()->Rotate(m_Bone1Rotation.x, m_Bone1Rotation.y, -m_Bone1Rotation.z * 2.f);
}

void SoftwareSkinningScene_1::OnGUI()
{
	//bone 0
	ImGui::PushItemWidth(70.0f);
	ImGui::DragFloat("##1", &m_Bone0Rotation.x);
	ImGui::SameLine();
	ImGui::DragFloat("##2", &m_Bone0Rotation.y);
	ImGui::SameLine();
	ImGui::DragFloat("bone 0 - ROT", &m_Bone0Rotation.z);

	//bone 1
	ImGui::DragFloat("##3", &m_Bone1Rotation.x);
	ImGui::SameLine();
	ImGui::DragFloat("##4", &m_Bone1Rotation.y);
	ImGui::SameLine();
	ImGui::DragFloat("bone 1 - ROT", &m_Bone1Rotation.z);
	ImGui::PopItemWidth();

	ImGui::Checkbox("Auto Rotate", &m_IsAutoRotateOn);
}
