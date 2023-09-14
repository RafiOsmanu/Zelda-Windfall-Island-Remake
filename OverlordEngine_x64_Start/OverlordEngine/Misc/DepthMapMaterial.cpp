#include "stdafx.h"
#include "DepthMapMaterial.h"

DepthMapMaterial::DepthMapMaterial() : Material(L"Effects/Depth/DepthMapGenerator.fx")
{}

void DepthMapMaterial::InitializeEffectVariables()
{
}
