#pragma once
class LoadingScreenScene : public GameScene
{
public:
	//rule of 5
	LoadingScreenScene() :GameScene(L"LoadingScreenScene") {}
	~LoadingScreenScene() override = default;
	LoadingScreenScene(const LoadingScreenScene& other) = delete;
	LoadingScreenScene(LoadingScreenScene&& other) noexcept = delete;
	LoadingScreenScene& operator=(const LoadingScreenScene& other) = delete;
	LoadingScreenScene& operator=(LoadingScreenScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;
private:
	float m_LoadTimer{};
	const float m_MaxLoadTime{ 8.f };

	GameObject* m_pLoadingScreenSprite{};

	//sounds
	FMOD::Channel* m_pMusicChannel{};
};

