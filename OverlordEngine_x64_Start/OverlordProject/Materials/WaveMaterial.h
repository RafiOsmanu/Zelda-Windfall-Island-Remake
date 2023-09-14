#pragma once
class WaveMaterial : public Material<WaveMaterial>
{
public:
	WaveMaterial();
	~WaveMaterial() = default;
	WaveMaterial(const WaveMaterial& other) = delete;
	WaveMaterial(WaveMaterial&& other) noexcept = delete;
	WaveMaterial& operator=(const WaveMaterial& other) = delete;
	WaveMaterial& operator=(WaveMaterial&& other) noexcept = delete;
		
	void InitializeEffectVariables() override;

	//set noise texture
	void SetNoiseTexture(const std::wstring& assetFile);

	//set distort texture
	void SetDistortTexture(const std::wstring& assetFile);

	//update time
	void Update(float elapsedTime);
	//set speed
	void SetSpeed(float speed) { m_Speed = speed; }

protected:
	void OnUpdateModelVariables(const SceneContext&, const ModelComponent*) const override;

private:
	TextureData* m_pNoiseTexture{};
	TextureData* m_pDistortTexture{};

	//time
	float m_Time = 0.f;
	//speed
	float m_Speed = 5.f;

};

