#pragma once
class BoneObject;

class SoftwareSkinningScene_2 : public GameScene
{
public:
	SoftwareSkinningScene_2();
	~SoftwareSkinningScene_2() override = default;

	SoftwareSkinningScene_2(const SoftwareSkinningScene_2& other) = delete;
	SoftwareSkinningScene_2(SoftwareSkinningScene_2&& other) noexcept = delete;
	SoftwareSkinningScene_2& operator=(const SoftwareSkinningScene_2& other) = delete;
	SoftwareSkinningScene_2& operator=(SoftwareSkinningScene_2&& other) noexcept = delete;

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
		VertexSoftwareSkinned(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT4 color)
			: transformedVertex{ position, normal, color },
			originalVertex{ position, normal, color } {}

		VertexPosNormCol transformedVertex{};
		VertexPosNormCol originalVertex{};

	};

	void InitializeVariables(float length);

	MeshDrawComponent* m_pMeshDrawer{};
	std::vector<VertexSoftwareSkinned> m_SkinnedVertices{};

};
