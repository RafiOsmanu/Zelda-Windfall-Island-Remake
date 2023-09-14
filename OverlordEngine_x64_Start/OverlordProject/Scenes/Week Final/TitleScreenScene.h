#pragma once
class ZeldaWaveMaterial;
class TitleScreenScene : public GameScene
{
public:
	TitleScreenScene() :GameScene(L"TitleScreenScene") {}
	~TitleScreenScene() override = default;
	TitleScreenScene(const TitleScreenScene& other) = delete;
	TitleScreenScene(TitleScreenScene&& other) noexcept = delete;
	TitleScreenScene& operator=(const TitleScreenScene& other) = delete;
	TitleScreenScene& operator=(TitleScreenScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;
private:
	enum InputIds
	{
		StartGame,
		QuitGame
	};

	// WaveCreation
	void CreateOceanWaves();

	//Level Creation
	void CreateLevel();

	PxMaterial* m_pDefaultMaterial{};
	ZeldaWaveMaterial* m_pWaveMaterial{};
	XMFLOAT3 m_IslandPos{ 0.f, 0.f, 0.f };

	// sounds
	FMOD::Channel* m_pMusicChannel{};

	//Sprites
	GameObject* m_pGameTitleSprite{};
	GameObject* m_pDaeBannerSprite{};
	GameObject* m_pPressStartSprite{};

	//Camera
	CameraComponent* m_pCamComponent{};

	bool m_IsSfxPlaying{ false };
	bool m_QuitGame{ false };


};

