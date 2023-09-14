#pragma once
#include "Materials/GeometryMaterial.h"
class GeometryShaderScene : public GameScene
{
public:
	GeometryShaderScene();
	~GeometryShaderScene() override = default;

	GeometryShaderScene(const GeometryShaderScene& other) = delete;
	GeometryShaderScene(GeometryShaderScene&& other) noexcept = delete;
	GeometryShaderScene& operator=(const GeometryShaderScene& other) = delete;
	GeometryShaderScene& operator=(GeometryShaderScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	GameObject* m_pSphere{};
	GeometryMaterial* m_pSphereMaterial{};

};

