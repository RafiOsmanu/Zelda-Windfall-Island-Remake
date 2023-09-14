#include "stdafx.h"
#include "ZeldaWaveMaterial.h"

//constructor
ZeldaWaveMaterial::ZeldaWaveMaterial() : Material(L"Effects/Depth/ZeldaWaterShader.fx")
{
}

void ZeldaWaveMaterial::InitializeEffectVariables()
{
}

void ZeldaWaveMaterial::Update(float elapsedTime)
{
	//update time
	m_Time += elapsedTime * m_Speed;
	//if (m_Time >= 100.f) m_Time = 0.f;
	SetVariable_Scalar(L"gTime", m_Time);
}
