#pragma once
class Link;
class Barrel : public GameObject
{
public:
	//rule of 5 based on inherited class
	Barrel(Link* pLink);
	~Barrel() override = default;

	Barrel(const Barrel& other) = delete;
	Barrel(Barrel&& other) noexcept = delete;
	Barrel& operator=(const Barrel& other) = delete;
	Barrel& operator=(Barrel&& other) noexcept = delete;
	GameObject* GetTriggerBox() const { return m_pTriggerBox; }

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;
private:
	GameObject* m_pTriggerBox;
	GameObject* m_pMeshCollider;
	GameObject* m_pBarrelObj;

	ModelComponent* m_pBarrelModel;

	Link* m_pLink;
	bool m_BarrelHit{false};
	bool m_BarrelDestroyed{ false };
	

	ParticleEmitterComponent* m_pEmitter{};

	float m_ParticleTimer{ 0.f };
	float m_ParticleTimerMax{ 4.f };
};

