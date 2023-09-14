#include "stdafx.h"
#include "BoneObject.h"
#include "Components/ModelComponent.h"


BoneObject::BoneObject(BaseMaterial* pMaterial, float length)
	: m_Length{length}
	, m_pMaterial{ pMaterial }
{
}

void BoneObject::Initialize(const SceneContext&)
{
	//1. Create an GameObject
	GameObject* pEmpty = new GameObject{};

	//2. Add pEmpty as a child to the BoneObject
	this->AddChild(pEmpty);

	//3. Add a ModelComponent to pEmpty
	const auto pModel = new ModelComponent(L"Meshes/Bone.ovm");
	pEmpty->AddComponent(pModel);

	//4. Assign the BoneObject’s material to pModel
	pModel->SetMaterial(m_pMaterial);

	//5. Rotate pEmpty -90 degrees around the Y-axis (This transform will orient the bone along the X-axis)
	pEmpty->GetTransform()->Rotate(0.f, -90.f, 0.f, true);

	//6. Uniformly scale pEmpty to match the BoneObject’s length
	pEmpty->GetTransform()->Scale(m_Length);
}

void BoneObject::AddBone(BoneObject* pBone)
{
	pBone->GetTransform()->Translate(m_Length, 0.f, 0.f);
	this->AddChild(pBone);

}

void BoneObject::CalculateBindPose()
{
	// Get the world matrix from the transform component
	XMFLOAT4X4 worldMatrix = this->GetTransform()->GetWorld();

	// Convert the world matrix to XMMatrix
	XMMATRIX worldMatrixXM = XMLoadFloat4x4(&worldMatrix);

	// Calculate the inverse of the world matrix
	XMMATRIX inverseWorldMatrixXM = XMMatrixInverse(nullptr, worldMatrixXM);

	// Convert the inverse world matrix back to XMFLOAT4X4
	XMFLOAT4X4 inverseWorldMatrix;
	XMStoreFloat4x4(&inverseWorldMatrix, inverseWorldMatrixXM);

	//store inverse matrix in bindPose
	m_BindPose = inverseWorldMatrix;
}

