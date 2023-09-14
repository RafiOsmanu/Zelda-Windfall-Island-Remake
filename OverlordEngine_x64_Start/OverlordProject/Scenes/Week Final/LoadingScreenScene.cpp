#include "stdafx.h"
#include "LoadingScreenScene.h"
#include "WindwakerScene.h"

void LoadingScreenScene::Initialize()
{
	//Loading screen
	auto loadingScreenSprite = AddChild(new GameObject());
	loadingScreenSprite->GetTransform()->Translate(m_SceneContext.windowWidth * 0.5f, m_SceneContext.windowHeight * 0.5f, .9f);

	m_pLoadingScreenSprite = loadingScreenSprite->AddChild(new GameObject);

	m_pLoadingScreenSprite->AddComponent(new SpriteComponent(L"Textures/TransitionScreen.jpg", XMFLOAT2(0.5f, 0.5f)));
	m_pLoadingScreenSprite->GetTransform()->Scale(1.f, 0.9f, 0.9f);
	m_SceneContext.settings.showInfoOverlay = false;
}

void LoadingScreenScene::Update()
{
	m_pMusicChannel->setVolume(0.1f);

	m_LoadTimer += m_SceneContext.pGameTime->GetElapsed();
	if (m_LoadTimer >= m_MaxLoadTime)
	{
		m_LoadTimer = 0.f;
		SceneManager::Get()->RemoveGameScene(L"WindwakerScene", true);
		SceneManager::Get()->AddGameScene(new WindwakerScene());
		SceneManager::Get()->SetActiveGameScene(L"WindwakerScene");
	}
}

void LoadingScreenScene::OnSceneActivated()
{
	auto pSoundSystem = SoundManager::Get()->GetSystem();
	FMOD::Sound* pSound{};
	pSoundSystem->createStream("Resources/Sounds/Music/LoadingScreenTheme.mp3", FMOD_2D, nullptr, &pSound);
	pSound->setMode(FMOD_LOOP_NORMAL);
	pSoundSystem->playSound(pSound, nullptr, false, &m_pMusicChannel);
}

void LoadingScreenScene::OnSceneDeactivated()
{
	m_pMusicChannel->stop();
}
