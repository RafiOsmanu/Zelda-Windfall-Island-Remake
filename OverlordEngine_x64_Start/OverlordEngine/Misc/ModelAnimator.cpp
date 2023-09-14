#include "stdafx.h"
#include "ModelAnimator.h"
#include <iostream>

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	TODO_W6_();

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		//auto passedTicks = ...
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		passedTicks = fmod(passedTicks, m_CurrentClip.duration);

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0)
			{
				m_TickCount += m_CurrentClip.duration;
			}
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.duration)
			{
				m_TickCount -= m_CurrentClip.duration;
			}
		}

		//3.
		// Find the enclosing keys
		AnimationKey keyA, keyB;
		keyA = m_CurrentClip.keys[0]; // Initialize with the first key
		keyB = m_CurrentClip.keys[0]; // Initialize with the first key

		// Iterate all the keys of the clip and find the following keys:
		// keyA > Closest Key with Tick before/smaller than m_TickCount
		// keyB > Closest Key with Tick after/bigger than m_TickCount
		for (const auto& key : m_CurrentClip.keys)
		{
			if (key.tick <= m_TickCount && key.tick > keyA.tick)
			{
				keyA = key;
			}
			else if (key.tick >= m_TickCount && key.tick < keyB.tick)
			{
				keyB = key;
			}
		}

		//4. Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		// Calculate the interpolation factor (blendFactor)
		float blendFactor{};
		float epsilon = 0.001f; // A small epsilon value
		if (keyB.tick - keyA.tick > epsilon) // Add epsilon to avoid division by zero
		{
			blendFactor = (m_TickCount - keyA.tick) / (keyB.tick - keyA.tick);
		}

		//Clear the m_Transforms vector
		m_Transforms.clear();

		//FOR every boneTransform in a key (So for every bone)
		for (size_t i = 0; i < keyA.boneTransforms.size(); i++)
		{
			const auto& transformA = keyA.boneTransforms[i]; // Retrieve transform from keyA
			const auto& transformB = keyB.boneTransforms[i]; // Retrieve transform from keyB

			// Decompose transforms into position, scale, and rotation components
			DirectX::XMVECTOR positionA, scaleA, rotationA;
			DirectX::XMMatrixDecompose(&scaleA, &rotationA, &positionA, XMLoadFloat4x4(&transformA));
			DirectX::XMVECTOR positionB, scaleB, rotationB;
			DirectX::XMMatrixDecompose(&scaleB, &rotationB, &positionB, XMLoadFloat4x4(&transformB));

			// Lerp between all the transformations (Position, Scale, Rotation)
			DirectX::XMVECTOR positionLerp = DirectX::XMVectorLerp(positionA, positionB, blendFactor);
			DirectX::XMVECTOR scaleLerp = DirectX::XMVectorLerp(scaleA, scaleB, blendFactor);
			DirectX::XMVECTOR rotationLerp = DirectX::XMQuaternionSlerp(rotationA, rotationB, blendFactor);


			// Compose a transformation matrix with the lerp-results
			DirectX::XMMATRIX transformLerp = DirectX::XMMatrixAffineTransformation(scaleLerp, DirectX::XMVECTOR{ 0 }, rotationLerp, positionLerp);

			XMFLOAT4X4 result;
			XMStoreFloat4x4(&result, transformLerp);

			// Add the resulting matrix to the m_Transforms vector
			m_Transforms.push_back(result);
		}


	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	TODO_W6_()
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for (const auto& clip : m_pMeshFilter->m_AnimationClips)
	{
		//If found,
		if (clip.name == clipName)
		{
			//Call SetAnimation(Animation Clip) with the found clip
			SetAnimation(clip);
			return;
		}
		else
		{
			//Call Reset
			Reset();

			//Log a warning with an appropriate message
			std::cout << "clip name doesn't match desired clip name!" << "\n";

		}
	}
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	TODO_W6_()
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (!(clipNumber < m_pMeshFilter->m_AnimationClips.size()))
	{
		//Call Reset
		Reset();
		std::cout << "clip number is bigger then animationclip size!" << "\n";
		return;
	}
	else
	{
		//Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//Call SetAnimation(AnimationClip clip)
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
	}
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	TODO_W6_()
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;

	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	TODO_W6_()
	//If pause is true, set m_IsPlaying to false
	if (pause)
	{
		m_IsPlaying = false;
	}

	//Set m_TickCount to zero
	m_TickCount = 0.f;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;
	//If m_ClipSet is true
	if (m_ClipSet)
	{
		//Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		//Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
		m_Transforms.assign(m_CurrentClip.keys[0].boneTransforms.begin(), (m_CurrentClip.keys[0].boneTransforms.end()));

	}
	else
	{
		//Create an IdentityMatrix 
		DirectX::XMFLOAT4X4 identityMatrix;
		DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());
		//Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identityMatrix);

	}
	
}
