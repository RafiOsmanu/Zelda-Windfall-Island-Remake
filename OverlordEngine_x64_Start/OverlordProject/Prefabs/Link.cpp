#include "stdafx.h"
#include "Link.h"
#include <iostream>
#include "Materials/ColorMaterial.h"
#include "TextureUtils.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "SoundManager/SoundEffectManager.h"

Link::Link(CharacterDesc& characterDesc) : 
	Character(characterDesc){}


void Link::Initialize(const SceneContext& sceneContext)
{
	Character::Initialize(sceneContext);

	const auto pModel = Character::SetModel(new ModelComponent(L"Link.ovm"));

	pModel->GetTransform()->Scale({ 0.08f, 0.08f, 0.08f });
	pModel->GetTransform()->Rotate(0.f, 180.f, 0.f);
	pModel->GetTransform()->Translate(0.f, -0.9f, 0.f);

	#pragma region AssingMaterials
		std::ifstream objFile(L"Resources/Meshes/link.obj");
	
		std::ifstream mtlFile(L"Resources/Textures/Link.mtl");
	
		std::wstring folderName(L"Link");
	
		textureUtils::AssignMaterials<DiffuseMaterial_Shadow_Skinned>(pModel, objFile, mtlFile, folderName);
	#pragma endregion

	//Animations 
	m_pAnimator = pModel->GetAnimator();
	m_pAnimator->SetAnimation((int)m_CurrCharacterState);
	m_pAnimator->SetAnimationSpeed(m_AnimationSpeed);
	m_pAnimator->Play();

	
	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,1.0f,0.f };
	settings.minSize = .2f;
	settings.maxSize = .4f;
	settings.minEnergy = .5f;
	settings.maxEnergy = .8f;
	settings.minScale = 2.f;
	settings.maxScale = 3.f;
	settings.minEmitterRadius = .05f;
	settings.maxEmitterRadius = .02f;
	settings.color = { 0.65f,0.65f,0.65f, .4f };

	const auto pObject = AddChild(new GameObject);
	m_pEmitter = pObject->AddComponent(new ParticleEmitterComponent(L"Textures/Dust.png", settings, 20));
	m_pEmitter->SetEnabled(false);

	//Sound
	InitSounds();
	

	SetTag(L"Link");
}

void Link::Update(const SceneContext& sceneContext)
{
	Character::Update(sceneContext);
	m_pEmitter->GetTransform()->Translate(GetFootPosition());
	//std::cout << GetFootPosition().x << ", " << GetFootPosition().y << ", " << GetFootPosition().z << std::endl;
	//std::cout << m_pEmitter->GetTransform()->GetPosition().x << ", " << m_pEmitter->GetTransform()->GetPosition().y << ", " << m_pEmitter->GetTransform()->GetPosition().z << std::endl;

	switch (m_CurrCharacterState)
	{
	case CharacterState::Idle:
		break;
	case CharacterState::Run:
		m_pAnimator->SetAnimationSpeed(1.5f);
		break;
	case CharacterState::Roll:

		m_pEmitter->SetEnabled(true);
		m_pAnimator->SetAnimationSpeed(1.6f);
		if (m_pAnimator->IsAnimComplete())
		{
			//m_pEmitter->SetEnabled(false);
			m_CurrCharacterState = CharacterState::Idle;
			m_SoundPlayed = false;
			m_pAnimator->SetAnimationSpeed(1.f);
		}
		break;
	case CharacterState::Jump:
		m_pAnimator->SetAnimationSpeed(1.f);

		if (m_pAnimator->IsAnimComplete())
		{
			m_CurrCharacterState = CharacterState::Falling;
		}
		break;
	case CharacterState::Falling:
		break;
	}

	if (IsCharacterStateChanged())
	{
		m_pEmitter->SetEnabled(false);
		m_pAnimator->SetAnimation((int)m_CurrCharacterState);

		//Roll Sound
		if (m_CurrCharacterState == CharacterState::Roll)
		{
			//SoundEffectManager::Get()->GetSfxChannel()->setVolume(0.f);
			SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::LinkRoll);
			int rndNumb = rand() % 3;

			switch (rndNumb)
			{
			case 0:
				SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::LinkRollVoice);
				break;
			case 1:
				SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::LinkRollVoice1);
				break;
			case 2:
				SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::LinkRollVoice2);
				break;
			}
		}

		//JumpSound
		if (m_CurrCharacterState == CharacterState::Jump)
		{
			//SoundEffectManager::Get()->GetSfxChannel()->setVolume(0.2f);
			int rndNumb = rand() % 3;

			switch (rndNumb)
			{
			case 0:
				SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::LinkHop);
				break;
			case 1:
				SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::LinkHop1);
				break;
			case 2:
				SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::LinkHop2);
				break;
			}
		}

	}
	SoundEffectManager::Get()->GetSfxChannel()->setVolume(0.3f);
}

void Link::InitSounds()
{
	auto pSoundEffectManager = SoundEffectManager::Get();
	pSoundEffectManager->AddSoundEffect(SoundEffectManager::SoundEffect::LinkRoll, "Resources/Sounds/SFX/WW_Link_Roll.wav");
	pSoundEffectManager->AddSoundEffect(SoundEffectManager::SoundEffect::LinkRollVoice, "Resources/Sounds/SFX/WW_Link_RollVoice.wav");
	pSoundEffectManager->AddSoundEffect(SoundEffectManager::SoundEffect::LinkRollVoice1, "Resources/Sounds/SFX/WW_Link_RollVoice1.wav");
	pSoundEffectManager->AddSoundEffect(SoundEffectManager::SoundEffect::LinkRollVoice2, "Resources/Sounds/SFX/WW_Link_RollVoice2.wav");
	pSoundEffectManager->AddSoundEffect(SoundEffectManager::SoundEffect::LinkHop, "Resources/Sounds/SFX/WW_Link_Hop.wav");
	pSoundEffectManager->AddSoundEffect(SoundEffectManager::SoundEffect::LinkHop1, "Resources/Sounds/SFX/WW_Link_Hop1.wav");
	pSoundEffectManager->AddSoundEffect(SoundEffectManager::SoundEffect::LinkHop2, "Resources/Sounds/SFX/WW_Link_Hop2.wav");
}

void Link::DrawImGui()
{
	Character::DrawImGui();
	if (ImGui::SliderFloat("Animation Speed", &m_AnimationSpeed, 0.f, 4.f))
	{
		m_pAnimator->SetAnimationSpeed(m_AnimationSpeed);
	}
}