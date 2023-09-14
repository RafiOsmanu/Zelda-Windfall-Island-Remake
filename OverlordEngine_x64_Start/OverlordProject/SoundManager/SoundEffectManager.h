#pragma once
#include "Utils/Singleton.h"
class SoundEffectManager final : public Singleton<SoundEffectManager>
{
public:

	~SoundEffectManager() = default;
	SoundEffectManager(const SoundEffectManager& other) = delete;
	SoundEffectManager(SoundEffectManager&& other) noexcept = delete;
	SoundEffectManager& operator=(const SoundEffectManager& other) = delete;
	SoundEffectManager& operator=(SoundEffectManager&& other) noexcept = delete;

	enum class SoundEffect
	{
		LinkRoll,
		LinkRollVoice,
		LinkRollVoice1,
		LinkRollVoice2,
		LinkHop,
		LinkHop1,
		LinkHop2,
		BarrelBreak,
		MenuOpen,
		MenuQuit,
		MenuSelect,
		PressStart,
		MenuBack,
		MenuClose,
		MenuCursor,
		GameWon
	};


	void AddSoundEffect(SoundEffect sfxId, const std::string& path);

	void DeleteSound(SoundEffect sfxId);

	void PlaySoundEffect(SoundEffect sfxId);

	void InitializeSfxSystem();
	FMOD::Channel* GetSfxChannel() { return m_pSfxChannel; }

protected:

	void Initialize() override {};

private:

	friend Singleton<SoundEffectManager>;
	SoundEffectManager() {};

	FMOD::Channel* m_pSfxChannel{};
	FMOD::System* m_pSystem{};
	std::unordered_map<SoundEffect, FMOD::Sound*> m_SfxMap{};
};






