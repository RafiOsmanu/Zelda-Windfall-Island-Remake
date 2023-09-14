#pragma once
class PostDim : public PostProcessingMaterial
{
public:
	PostDim();
	~PostDim() override = default;
	PostDim(const PostDim& other) = delete;
	PostDim(PostDim&& other) noexcept = delete;
	PostDim& operator=(const PostDim& other) = delete;
	PostDim& operator=(PostDim&& other) noexcept = delete;

	void SetDimming(float dimming) const;
protected:
	void Initialize(const GameContext& /*gameContext*/) override {};
};

