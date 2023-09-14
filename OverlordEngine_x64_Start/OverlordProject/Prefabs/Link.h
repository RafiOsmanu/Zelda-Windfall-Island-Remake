#pragma once
#include "Character.h"
class Link : public  Character
{
public:
	//rule of 5
	Link(CharacterDesc& characterDesc);
	~Link() override = default;
		
	Link(const Link& other) = delete;
	Link(Link&& other) noexcept = delete;
	Link& operator=(const Link& other) = delete;
	Link& operator=(Link&& other) noexcept = delete;

	void DrawImGui();
	CharacterState GetCurrentState() const { return m_CurrCharacterState; }
	void SetCurrentState(CharacterState state) { m_CurrCharacterState = state; }
	ModelAnimator* GetAnimator() const { return m_pAnimator; }
	int GetAmountOfBarrelsDestroyed() const { return m_AmountOfBarrelsDestroyed; }
	int GetAmountOfBarrels() const { return m_AmountOfBarrels; }

	void BarrelDestroyed() { m_AmountOfBarrelsDestroyed++; }
	bool GameWon() { return m_AmountOfBarrelsDestroyed >= m_AmountOfBarrels; };


protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;
private:
	void InitSounds();
	ModelAnimator* m_pAnimator;
	ParticleEmitterComponent* m_pEmitter{};
	float m_AnimationSpeed{ 4.f };

	int m_AmountOfBarrelsDestroyed{0};
	int m_AmountOfBarrels{ 15 };

	bool m_SoundPlayed{ false };
	

};

