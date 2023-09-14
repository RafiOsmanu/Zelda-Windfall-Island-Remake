#pragma once
class DepthMapMaterial;

class CameraDepthRenderer : public Singleton<CameraDepthRenderer>
{
public:
	CameraDepthRenderer(const CameraDepthRenderer& other) = delete;
	CameraDepthRenderer(CameraDepthRenderer&& other) noexcept = delete;
	CameraDepthRenderer& operator=(const CameraDepthRenderer& other) = delete;
	CameraDepthRenderer& operator=(CameraDepthRenderer&& other) noexcept = delete;

	void UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const;

	void Begin(const SceneContext&);
	void DrawMesh(const SceneContext& sceneContext, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones = {});
	void End(const SceneContext&) const;

	ID3D11ShaderResourceView* GetDepthMap() const;
	const XMFLOAT4X4& GetCameraVP() const { return m_CameraVP; }

	void Debug_DrawDepthSRV(const XMFLOAT2& position = { 0.f,0.f }, const XMFLOAT2& scale = { 1.f,1.f }, const XMFLOAT2& pivot = { 0.f,0.f }) const;

protected:
	void Initialize() override;

private:
	friend class Singleton<CameraDepthRenderer>;
	CameraDepthRenderer() = default;
	~CameraDepthRenderer();

	//Rendertarget to render the 'depth Map' to (depth-only)
	//Contains depth information for all rendered meshes from a camera's perspective 
	RenderTarget* m_pCameraRenderTarget{ nullptr };

	//Camera ViewProjection (perspective used to render depthMap)
	XMFLOAT4X4 m_CameraVP{};

	//CameraDepth Generator is responsible of drawing all meshes to the DepthMap
	//There are two techniques, one for static (non-skinned) meshes, and another for skinned meshes (with bones, blendIndices, blendWeights)
	enum class DepthGeneratorType
	{
		Static,
		Skinned
	};

	DepthMapMaterial* m_pDepthMapGenerator{ nullptr };

	//Information about each technique (static/skinned) is stored in a MaterialTechniqueContext structure
	//This information is automatically create by the Material class, we only store it in a local array for fast retrieval 
	static int const NUM_TYPES{ 2 };
	MaterialTechniqueContext m_GeneratorTechniqueContexts[NUM_TYPES];



};

