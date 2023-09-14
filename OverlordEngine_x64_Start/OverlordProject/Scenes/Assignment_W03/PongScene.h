#include "Prefabs/CubePrefab.h"
#pragma once
class PongScene final : public GameScene
{
public:
	PongScene();
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	enum InputIds
	{
		p1Up, p1Down, p2Up, p2Down
	};

	RigidBodyComponent* m_pPlayer1;
	RigidBodyComponent* m_pPlayer2;
	PxRigidStatic* m_pPalletTrigger1;
	PxRigidStatic* m_pPalletTrigger2;
	//float m_Transportation;
	
};

