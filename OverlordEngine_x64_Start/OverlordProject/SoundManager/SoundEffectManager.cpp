#include "stdafx.h"
#include "SoundEffectManager.h"


void SoundEffectManager::AddSoundEffect(SoundEffect sfxId, const std::string& path)
{
	if (m_SfxMap.contains(sfxId))
		return;

	FMOD::Sound* pSfx{};
	SUCCEEDED(m_pSystem->createStream(path.c_str(), FMOD_DEFAULT, nullptr, &pSfx));
	m_SfxMap.insert({ sfxId, pSfx });
}

void SoundEffectManager::DeleteSound(SoundEffect sfxId)
{
	if (!m_SfxMap.contains(sfxId))
		return;
	FMOD::Sound* pSfx = m_SfxMap.at(sfxId);
	pSfx->release();
	m_SfxMap.erase(sfxId);
}

void SoundEffectManager::PlaySoundEffect(SoundEffect sfxId)
{
	if (!m_SfxMap.contains(sfxId))
	{
		std::cout << " no sfx found with id: " << (UINT)sfxId << "!" << std::endl;
		return;
	}


	FMOD::Sound* pSound = m_SfxMap.at(sfxId);

	if (pSound == nullptr)
	{
		std::cout << " No sfx found with id: " << (UINT)sfxId << "!" << std::endl;
		return;
	}

	m_pSystem->playSound(pSound, nullptr, false, &m_pSfxChannel);
	
}

void SoundEffectManager::InitializeSfxSystem()
{
	m_pSystem = SoundManager::Get()->GetSystem();

}


