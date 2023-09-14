#pragma once
class GeometryMaterial : public Material<GeometryMaterial>
{
public:
	GeometryMaterial();
	~GeometryMaterial() override = default;

	GeometryMaterial(const GeometryMaterial& other) = delete;
	GeometryMaterial(GeometryMaterial&& other) noexcept = delete;
	GeometryMaterial& operator=(const GeometryMaterial& other) = delete;
	GeometryMaterial& operator=(GeometryMaterial&& other) noexcept = delete;

	void InitializeEffectVariables() override;
protected:
private:
	TextureData* m_pDiffuseTexture{};

};

