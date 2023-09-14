#pragma once
class Character;
class ZeldaWaveMaterial;
class WaveMaterial;
class Link;
class Barrel;
class PostDim;


class WindwakerScene : public GameScene
{
public:
	WindwakerScene() :GameScene(L"WindwakerScene") {}
	~WindwakerScene() override = default;
	WindwakerScene(const WindwakerScene& other) = delete;
	WindwakerScene(WindwakerScene&& other) noexcept = delete;
	WindwakerScene& operator=(const WindwakerScene& other) = delete;
	WindwakerScene& operator=(WindwakerScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;
	void PostDraw() override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;
private:

	PxMaterial* m_pDefaultMaterial{};

	//WaveCreation
	void CreateOceanWaves();

	//CharacterCreation
	void CreateCharacter();
	ModelComponent* m_pLinkModel{};

	//LevelCreation 
	void CreateLevel();

	//INPUT

	void SetUpInput();

	//setup barrels
	void CreateBarrels();

	//UI setup
	void SetupUI();

	//pauze game
	void PauzeGame();

	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump,
		CharacterRoll,
		MenuPauze,
		MenuRight,
		MenuLeft,
		MenuSelect
	};

	Character* m_pCharacter{};

	ZeldaWaveMaterial* m_pWaveMaterial{};

	Link* m_pLink{};
	GameObject* m_pBarrel{};

	bool m_DrawShadowMap{ false };

	float m_ShadowMapScale{ 1.3f };

	XMFLOAT3 m_CameraDir{ 0.f, -0.7f, .1f };
	XMFLOAT3 m_CameraPos{ 0.f, 50.f, 5.f };
	float m_LightDistance{ 60.f };

	XMFLOAT3 m_IslandPos{ 0.f, 0.f, 0.f };

	float posX{0.f};
	float posY{0.f};
	float posZ{0.f};
	Barrel* m_pBarrelTemp{};

	GameObject* m_pBarrelSprite{};
	GameObject* m_pQuitBoxSprite{};
	GameObject* m_pContinueBoxSprite{};
	GameObject* m_pControllsSprite{};

	//UI
	SpriteFont* m_pFont{};
	XMFLOAT2 m_TextPos{ 1085,600 };
	XMFLOAT4 m_TextColor{ 0.96f,0.96f,0.96f,1.f };

	// sounds
	FMOD::Channel* m_pMusicChannel{};
	bool m_GameWonPlayed{ false };
	
	//post
	PostDim* m_pPostDim{};

	int m_InPutId{1};
	void CheckInputId();
	bool m_IsSfxPlaying{ false };
	bool m_QuitGame{false};
};



