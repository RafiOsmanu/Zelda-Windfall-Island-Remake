#pragma once
class BoneObject;

class SoftwareSkinningScene_3 : public GameScene
{
public:
	SoftwareSkinningScene_3();
	~SoftwareSkinningScene_3() override = default;

	SoftwareSkinningScene_3(const SoftwareSkinningScene_3& other) = delete;
	SoftwareSkinningScene_3(SoftwareSkinningScene_3&& other) noexcept = delete;
	SoftwareSkinningScene_3& operator=(const SoftwareSkinningScene_3& other) = delete;
	SoftwareSkinningScene_3& operator=(SoftwareSkinningScene_3&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	BoneObject* m_pBone0{}, * m_pBone1{};

	XMFLOAT3 m_Bone0Rotation{};
	XMFLOAT3 m_Bone1Rotation{};

	int m_RotationSign{ 1 };
	bool m_IsAutoRotateOn{ false };

	//Part2 [ADDED]

	struct VertexSoftwareSkinned
	{
		VertexSoftwareSkinned(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT4 color, float blendWeight)
			: transformedVertex{ position, normal, color },
			originalVertex{ position, normal, color },
			blendWeight0{ blendWeight },
			blendWeight1{ 1.f - blendWeight } {}



		VertexPosNormCol transformedVertex{};
		VertexPosNormCol originalVertex{};

		float blendWeight0{}, blendWeight1{};


	};

	void InitializeVariables(float length);

	MeshDrawComponent* m_pMeshDrawer{};
	std::vector<VertexSoftwareSkinned> m_SkinnedVertices{};

};

