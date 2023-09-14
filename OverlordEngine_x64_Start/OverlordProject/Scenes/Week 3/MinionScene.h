#pragma once
#include "Prefabs/TorusPrefab.h"
class MinionScene final : public GameScene
{
public:
	MinionScene();
	~MinionScene() override = default;

	MinionScene(const MinionScene& other) = delete;
	MinionScene(MinionScene&& other) noexcept = delete;
	MinionScene& operator=(const MinionScene& other) = delete;
	MinionScene& operator=(MinionScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	
	TorusPrefab* m_pTorusSmall;
	TorusPrefab* m_pTorusMid;
	TorusPrefab* m_pTorusBig;
	float m_Speed;


};


