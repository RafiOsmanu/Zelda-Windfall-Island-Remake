#include "stdafx.h"
#include "SoftwareSkinningScene_3.h"
#include "Content/PxMeshLoader.h"
#include "Materials/ColorMaterial.h"
#include "Managers/MaterialManager.h"
#include "Components/ModelComponent.h"
#include "Prefabs/BoneObject.h"

SoftwareSkinningScene_3::SoftwareSkinningScene_3() : GameScene(L"SoftwareSkinningScene_3"){}

void SoftwareSkinningScene_3::Initialize()
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

	//create new gameobject 
	GameObject* pBoxDrawer = new GameObject();
	AddChild(pBoxDrawer);

	//calculate the bind pose once on bone0
	m_pBone0->CalculateBindPose();
	m_pBone1->CalculateBindPose();

	//make a new meshdrawcomponent and add it to boxdrawer
	m_pMeshDrawer = new MeshDrawComponent{ 24U, true };
	pBoxDrawer->AddComponent(m_pMeshDrawer);

	InitializeVariables(15.f);
}

void SoftwareSkinningScene_3::Update()
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
	m_pBone1->GetTransform()->Rotate(m_Bone1Rotation.x, m_Bone1Rotation.y, -m_Bone1Rotation.z * 2);


	//store in xm matrix
	CXMMATRIX bindPoseBone0 = XMLoadFloat4x4(&m_pBone0->GetBindPose());
	CXMMATRIX bindPoseBone1 = XMLoadFloat4x4(&m_pBone1->GetBindPose());

	CXMMATRIX Bone0WorldMatrix = XMLoadFloat4x4(&m_pBone0->GetTransform()->GetWorld());
	CXMMATRIX Bone1WorldMatrix = XMLoadFloat4x4(&m_pBone1->GetTransform()->GetWorld());

	//calculate relative transform
	XMMATRIX relativeTransformBone0 = XMMatrixMultiply(bindPoseBone0, Bone0WorldMatrix);
	XMMATRIX relativeTransformBone1 = XMMatrixMultiply(bindPoseBone1, Bone1WorldMatrix);


	int tempIndex{ 0 };

	for (auto& vertex : m_SkinnedVertices)
	{
		

		XMMATRIX lerp =  (relativeTransformBone0 * vertex.blendWeight0) + (relativeTransformBone1 * vertex.blendWeight1);

		XMVECTOR vPos = XMLoadFloat3(&vertex.originalVertex.Position);
		//XMVECTOR xmResult = XMVector3TransformCoord(vPos, tempIndex < 24 ? relativeTransformBone0 : relativeTransformBone1);

		XMVECTOR xmResult = XMVector3TransformCoord(vPos, lerp);

		XMFLOAT3 result;
		XMStoreFloat3(&result, xmResult);

		vertex.transformedVertex.Position = result;

		++tempIndex;

	}

	//draw boxes
	m_pMeshDrawer->RemoveTriangles();

	for (unsigned int i{}; i < m_SkinnedVertices.size(); i += 4)
	{
		QuadPosNormCol newQuad = QuadPosNormCol
		{
			m_SkinnedVertices[i].transformedVertex,
			m_SkinnedVertices[i + 1].transformedVertex,
			m_SkinnedVertices[i + 2].transformedVertex,
			m_SkinnedVertices[i + 3].transformedVertex
		};

		m_pMeshDrawer->AddQuad(newQuad);
	}

	m_pMeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene_3::OnGUI()
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

void SoftwareSkinningScene_3::InitializeVariables(float length)
{
	auto pos = XMFLOAT3(length / 2.f, 0.f, 0.f);
	const auto offset = XMFLOAT3(length / 2.f, 2.5f, 2.5f);
	auto col = XMFLOAT4(1.f, 0.f, 0.f, 0.5f);
#pragma region BOX 1
	//FRONT
	XMFLOAT3 norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, .5f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f);
#pragma endregion

	col = { 0.f, 1.f, 0.f, 0.5f };
	pos = { 22.5f, 0.f, 0.f };
#pragma region BOX 2
	//FRONT
	norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, .5f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
#pragma endregion
}
