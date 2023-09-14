#pragma once
class DepthMapMaterial final: public Material<DepthMapMaterial>
{
public:
	DepthMapMaterial();
	~DepthMapMaterial() override = default;
	DepthMapMaterial(const DepthMapMaterial& other) = delete;
	DepthMapMaterial(DepthMapMaterial&& other) noexcept = delete;
	DepthMapMaterial& operator=(const DepthMapMaterial& other) = delete;
	DepthMapMaterial& operator=(DepthMapMaterial&& other) noexcept = delete;
protected:
	void InitializeEffectVariables() override;
};

