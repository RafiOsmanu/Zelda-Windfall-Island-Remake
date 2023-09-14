#pragma once
class ComponentTestscene final : public GameScene
{
public:
	ComponentTestscene();
	~ComponentTestscene() override = default;

	ComponentTestscene(const ComponentTestscene& other) = delete;
	ComponentTestscene(ComponentTestscene&& other) noexcept = delete;
	ComponentTestscene& operator=(const ComponentTestscene& other) = delete;
	ComponentTestscene& operator=(ComponentTestscene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

};
