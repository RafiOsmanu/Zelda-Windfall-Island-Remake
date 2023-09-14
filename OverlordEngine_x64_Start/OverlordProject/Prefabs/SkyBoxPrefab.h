#pragma once
#include "Materials/SkyBoxMaterial.h"
class SkyBoxPrefab : public GameObject
{
public:
	SkyBoxPrefab(const std::wstring& textureName);
	~SkyBoxPrefab() override = default;

	SkyBoxPrefab(const SkyBoxPrefab& other) = delete;
	SkyBoxPrefab(SkyBoxPrefab&& other) noexcept = delete;
	SkyBoxPrefab& operator=(const SkyBoxPrefab& other) = delete;
	SkyBoxPrefab& operator=(SkyBoxPrefab&& other) noexcept = delete;

	void Initialize(const SceneContext& /*sceneContext*/) override;

private:
	ModelComponent* m_pSkyBoxModel{};
	const std::wstring m_pSkyBoxTextureName;
	SkyBoxMaterial* m_pSkyBoxMaterial;
};

