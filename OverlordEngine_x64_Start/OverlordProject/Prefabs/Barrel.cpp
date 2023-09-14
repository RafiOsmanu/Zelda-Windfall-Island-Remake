#include "stdafx.h"
#include "Barrel.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/ColorMaterial.h"
#include "TextureUtils.h"
#include "Prefabs/Link.h"
#include "SoundManager/SoundEffectManager.h"

Barrel::Barrel(Link* pLink) : m_pLink{pLink}
{
}

void Barrel::Initialize(const SceneContext& /*sceneContext*/)
{
	auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);


	m_pBarrelObj = AddChild(new GameObject());
	m_pBarrelModel = m_pBarrelObj->AddComponent(new ModelComponent(L"Meshes/Barrel.ovm"));
	//pModel->GetTransform()->Rotate(90.f, 0.f, 0.f);

	std::ifstream objFile(L"Resources/Meshes/barrel.obj");

	std::ifstream mtlFile(L"Resources/Textures/barrel.mtl");

	std::wstring folderName(L"");

	textureUtils::AssignMaterials<DiffuseMaterial_Shadow>(m_pBarrelModel, objFile, mtlFile, folderName);

	//TriggerBox
	m_pTriggerBox = AddChild(new GameObject());
	auto pTriggerActor = m_pTriggerBox->AddComponent(new RigidBodyComponent(true));
	pTriggerActor->AddCollider(PxBoxGeometry(1.f, 1.5f, 1.f), *pDefaultMaterial, true);

	

	m_pTriggerBox->SetOnTriggerCallBack([&](GameObject* /*trigger*/ , GameObject* other, PxTriggerAction action)
	{
		if (other == m_pLink)
		{
			if (action == PxTriggerAction::ENTER && m_pLink->GetCurrentState() == Link::CharacterState::Roll)
			{
				if (!m_BarrelHit)
				{
					m_BarrelHit = true;
					m_pLink->BarrelDestroyed();
					m_pLink->SetCurrentState(Link::CharacterState::Idle);
					m_pLink->GetAnimator()->SetAnimation((int)Link::CharacterState::Idle);
				}
			}
			
		}
		
	});


	//MeshColider
	m_pMeshCollider = AddChild(new GameObject());
	const auto pBarrelTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Barrel.ovpt");

	auto pRigidBody = m_pMeshCollider->AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxTriangleMeshGeometry(pBarrelTriangleMesh, PxMeshScale({0.1f, 0.1f, 0.1f})), *pDefaultMaterial);

	//Scale
	m_pBarrelModel->GetTransform()->Scale(0.1f, 0.1f, 0.1f);

	//Tags
	SetTag(L"Barrel");

	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,4.0f,0.f };
	settings.minSize = .2f;
	settings.maxSize = .6f;
	settings.minEnergy = .5f;
	settings.maxEnergy = 1.8f;
	settings.minScale = 2.f;
	settings.maxScale = 3.f;
	settings.minEmitterRadius = .05f;
	settings.maxEmitterRadius = .58f;
	settings.color = { 0.45f,0.45f,0.45f, .4f };

	const auto pParticleObject = AddChild(new GameObject);
	m_pEmitter = pParticleObject->AddComponent(new ParticleEmitterComponent(L"Textures/Dust.png", settings, 30));
	m_pEmitter->SetEnabled(false);

	//SOUND
	SoundEffectManager::Get()->AddSoundEffect(SoundEffectManager::SoundEffect::BarrelBreak, "Resources/Sounds/SFX/WW_BarrelBreak.mp3");

}

void Barrel::Update(const SceneContext& sceneContext)
{
	if (!m_BarrelHit)
	{
		m_pTriggerBox->GetTransform()->Translate(XMFLOAT3{ GetTransform()->GetPosition().x, GetTransform()->GetPosition().y + 1, GetTransform()->GetPosition().z });
		m_pMeshCollider->GetTransform()->Translate(XMFLOAT3{ GetTransform()->GetPosition().x, GetTransform()->GetPosition().y, GetTransform()->GetPosition().z });
	}

	m_pEmitter->GetTransform()->Translate(GetTransform()->GetPosition());

	if (m_BarrelHit)
	{
		m_pEmitter->SetEnabled(true);
		m_ParticleTimer += sceneContext.pGameTime->GetElapsed();
		if (m_ParticleTimer >= m_ParticleTimerMax)
		{
			m_pEmitter->SetEnabled(false);

			GetParent()->RemoveChild(this, true);
		}

		if (!m_BarrelDestroyed)
		{
			m_BarrelDestroyed = true;
			SoundEffectManager::Get()->GetSfxChannel()->setVolume(0.2f);
			SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::BarrelBreak);
			m_pBarrelObj->RemoveComponent(m_pBarrelModel, true);
			RemoveChild(m_pTriggerBox, true);
			RemoveChild(m_pMeshCollider, true);
		}
	}
}
