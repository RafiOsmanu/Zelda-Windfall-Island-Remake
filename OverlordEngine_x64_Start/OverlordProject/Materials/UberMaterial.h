#pragma once
class UberMaterial : public Material<UberMaterial>
{
public:
	UberMaterial();
	~UberMaterial() override = default;

	UberMaterial(const UberMaterial& other) = delete;
	UberMaterial(UberMaterial&& other) noexcept = delete;
	UberMaterial& operator=(const UberMaterial& other) = delete;
	UberMaterial& operator=(UberMaterial&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);
protected:
	void InitializeEffectVariables() override;
private:
	TextureData* m_pDiffuseTexture{};
	TextureData* m_pSpecularTexture{};
	TextureData* m_pNormalTexture{};
	TextureData* m_pEnviorementMap{};
	
	

};

