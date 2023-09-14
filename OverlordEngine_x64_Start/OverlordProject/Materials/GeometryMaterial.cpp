#include "stdafx.h"
#include "GeometryMaterial.h"

GeometryMaterial::GeometryMaterial() 
	: Material(L"Effects/SpikyShader.fx") {}



void GeometryMaterial::InitializeEffectVariables()
{
	SetVariable_Scalar(L"gSpikeLength", 0.f);
}
