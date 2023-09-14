#include "stdafx.h"
#include "PostDim.h"

PostDim::PostDim() :
	PostProcessingMaterial(L"Effects/Post/Dim.fx")
{
}

void PostDim::SetDimming(float dimming) const
{
	m_pBaseEffect->GetVariableByName("gDim")->AsScalar()->SetFloat(dimming);
}
