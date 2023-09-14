#include "stdafx.h"
#include "CameraDepthRenderer.h"
#include "Misc/DepthMapMaterial.h"
#include <DirectXMath.h>

CameraDepthRenderer::~CameraDepthRenderer()
{
	SafeDelete(m_pCameraRenderTarget);
}

void CameraDepthRenderer::Initialize()
{
	//1. Create a separate RenderTarget (see RenderTarget class), store in m_pShadowRenderTarget
		//	- RenderTargets are initialized with the RenderTarget::Create function, requiring a RENDERTARGET_DESC
		//	- Initialize the relevant fields of the RENDERTARGET_DESC (enableColorBuffer:false, enableDepthSRV:true, width & height)


	RENDERTARGET_DESC renderDescb{};
	renderDescb.enableColorBuffer = false;
	renderDescb.enableDepthSRV = true;
	renderDescb.width = 8192;
	renderDescb.height = 8192;

	m_pCameraRenderTarget = new RenderTarget(m_GameContext.d3dContext);
	m_pCameraRenderTarget->Create(renderDescb);

	//2. Create a new ShadowMapMaterial, this will be the material that 'generated' the ShadowMap, store in m_pShadowMapGenerator
	//	- The effect has two techniques, one for static meshes, and another for skinned meshes (both very similar, the skinned version also transforms the vertices based on a given set of boneTransforms)
	//	- We want to store the TechniqueContext (struct that contains information about the Technique & InputLayout required for rendering) for both techniques in the m_GeneratorTechniqueContexts array.
	//	- Use the ShadowGeneratorType enum to retrieve the correct TechniqueContext by ID, and also use that ID to store it inside the array (see BaseMaterial::GetTechniqueContext)

	m_pDepthMapGenerator = MaterialManager::Get()->CreateMaterial<DepthMapMaterial>();

	const auto skinnedIndex = static_cast<int>(DepthGeneratorType::Skinned);
	const auto staticIndex = static_cast<int>(DepthGeneratorType::Static);

	m_GeneratorTechniqueContexts[skinnedIndex] = m_pDepthMapGenerator->GetTechniqueContext(skinnedIndex);
	m_GeneratorTechniqueContexts[staticIndex] = m_pDepthMapGenerator->GetTechniqueContext(staticIndex);
}

void CameraDepthRenderer::UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const
{
	TODO_W8(L"Implement UpdateMeshFilter")
	//Here we want to Update the MeshFilter of ModelComponents that need to be rendered to the ShadowMap
	//Updating the MeshFilter means that we want to create a corresponding VertexBuffer for our ShadowGenerator material

	//1. Figure out the correct ShadowGeneratorType (either Static, or Skinned) with information from the incoming MeshFilter
	//2. Retrieve the corresponding TechniqueContext from m_GeneratorTechniqueContexts array (Static/Skinned)
	//3. Build a corresponding VertexBuffer with data from the relevant TechniqueContext you retrieved in Step2 (MeshFilter::BuildVertexBuffer)

	const auto depthGeneratorType = pMeshFilter->HasAnimations() ? DepthGeneratorType::Skinned : DepthGeneratorType::Static;
	const auto techniqueContext = m_GeneratorTechniqueContexts[static_cast<int>(depthGeneratorType)];

	pMeshFilter->BuildVertexBuffer(sceneContext, techniqueContext.inputLayoutID, techniqueContext.inputLayoutSize, techniqueContext.pInputLayoutDescriptions);
}

void CameraDepthRenderer::Begin(const SceneContext& sceneContext)
{

	// Change the viewport to match the shadow map size 
	D3D11_VIEWPORT vp {};


	vp.Width = static_cast<float>(m_pCameraRenderTarget->GetDesc().width);
	vp.Height = static_cast<float>(m_pCameraRenderTarget->GetDesc().height);

	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;

	sceneContext.d3dContext.pDeviceContext->RSSetViewports(1, &vp);


	//1. Making sure that the ShadowMap is unbound from the pipeline as ShaderResourceView (SRV) is important, because we cannot use the same resource as a ShaderResourceView (texture resource inside a shader) and a RenderTargetView (target everything is rendered too) at the same time. In case this happens, you'll see an error in the output of visual studio - warning you that a resource is still bound as a SRV and cannot be used as an RTV.
	//	-> Unbinding an SRV can be achieved using DeviceContext::PSSetShaderResource [I'll give you the implementation for free] - double check your output because depending on your usage of ShaderResources, the actual slot the ShadowMap is using can be different, but you'll see a warning pop-up with the correct slot ID in that case.
	constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);


	m_CameraVP = sceneContext.pCamera->GetViewProjection();

	//3. Update this matrix (m_CameraVP) on the DepthMapMaterial effect
	m_pDepthMapGenerator->SetVariable_Matrix(L"gCameraViewProj", reinterpret_cast<const float*>(&m_CameraVP));
	

	//4. Set the Main Game RenderTarget to m_pShadowRenderTarget (OverlordGame::SetRenderTarget) - Hint: every Singleton object has access to the GameContext...
	m_GameContext.pGame->SetRenderTarget(m_pCameraRenderTarget);


	//5. Clear the ShadowMap rendertarget (RenderTarget::Clear)
	m_pCameraRenderTarget->Clear();
}

void CameraDepthRenderer::DrawMesh(const SceneContext& sceneContext, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones)
{

	const auto depthGeneratorType = pMeshFilter->HasAnimations() ? DepthGeneratorType::Skinned : DepthGeneratorType::Static;
	const auto techniqueContext = m_GeneratorTechniqueContexts[static_cast<int>(depthGeneratorType)];

	m_pDepthMapGenerator->SetVariable_Matrix(L"gWorld", reinterpret_cast<const float*>(&meshWorld));
	if (pMeshFilter->HasAnimations())
	{
		m_pDepthMapGenerator->SetVariable_MatrixArray(L"gBones", &meshBones[0]._11, (UINT)meshBones.size());
	}

	//Update Materials

	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	//Set Inputlayout
	pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);
	//Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (const auto& subMesh : pMeshFilter->GetMeshes())
	{
		//Set Vertex Buffer
		const UINT offset = 0;
		const auto& vertexBufferData = pMeshFilter->GetVertexBufferData(techniqueContext.inputLayoutID, subMesh.id);
		pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride,
			&offset);

		//Set Index Buffer
		pDeviceContext->IASetIndexBuffer(subMesh.buffers.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		m_pDepthMapGenerator->SetTechnique(static_cast<int>(depthGeneratorType));

		//DRAW
		auto& tech = techniqueContext.pTechnique;
		D3DX11_TECHNIQUE_DESC techDesc{};

		tech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			tech->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(subMesh.indexCount, 0, 0);
		}
	}
}

void CameraDepthRenderer::End(const SceneContext& sceneContext) const
{
	m_GameContext.pGame->SetRenderTarget(nullptr);

	// Restore the viewport size to the original
	D3D11_VIEWPORT vp{};
	vp.Width = static_cast<float>(sceneContext.windowWidth);
	vp.Height = static_cast<float>(sceneContext.windowHeight);
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	sceneContext.d3dContext.pDeviceContext->RSSetViewports(1, &vp);
}

ID3D11ShaderResourceView* CameraDepthRenderer::GetDepthMap() const
{
	return m_pCameraRenderTarget->GetDepthShaderResourceView();
}

void CameraDepthRenderer::Debug_DrawDepthSRV(const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	if (m_pCameraRenderTarget->HasDepthSRV())
	{
		SpriteRenderer::Get()->DrawImmediate(m_GameContext.d3dContext, m_pCameraRenderTarget->GetDepthShaderResourceView(), position, XMFLOAT4{ Colors::White }, pivot, scale);

		//Remove from Pipeline
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
		m_GameContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
}


