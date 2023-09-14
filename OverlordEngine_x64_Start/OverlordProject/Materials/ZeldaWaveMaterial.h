#pragma once
class ZeldaWaveMaterial final : public Material<ZeldaWaveMaterial>
{
public:
	ZeldaWaveMaterial();
	~ZeldaWaveMaterial() = default;
	ZeldaWaveMaterial(const ZeldaWaveMaterial& other) = delete;
	ZeldaWaveMaterial(ZeldaWaveMaterial&& other) noexcept = delete;
	ZeldaWaveMaterial& operator=(const ZeldaWaveMaterial& other) = delete;
	ZeldaWaveMaterial& operator=(ZeldaWaveMaterial&& other) noexcept = delete;

	void InitializeEffectVariables();

	void Update(float elapsedTime);
	void SetSpeed(float speed) { m_Speed = speed; }

private:

	float m_Time = 0.f;
	float m_Speed = 20.f;
};

