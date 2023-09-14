#pragma once
struct CharacterDesc
{
	CharacterDesc(
		PxMaterial* pMaterial,
		float radius = .3f ,
		float height = 1.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}

	float maxMoveSpeed{ 6.5f };
	float maxFallSpeed{ 0.7f };

	float JumpSpeed{ 0.4f };

	float moveAccelerationTime{ .25f };
	float fallAccelerationTime{ .6f };

	float maxCameraDist{ 10.f };
	float minCameraDist{ 4.f };

	float maxCameraHeight{ 4.f };
	float minCameraHeight{ 0.f };

	PxCapsuleControllerDesc controller{};

	float rotationSpeed{ 60.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_Jump{ -1 };
	int actionId_Roll{ -1 };
};

class Character : public GameObject
{
public:
	Character(const CharacterDesc& characterDesc);
	~Character() override = default;

	Character(const Character& other) = delete;
	Character(Character&& other) noexcept = delete;
	Character& operator=(const Character& other) = delete;
	Character& operator=(Character&& other) noexcept = delete;

	enum class CharacterState
	{
		Idle,
		Run,
		Roll,
		Jump,
		Falling
	};

	void DrawImGui();
	ModelComponent* SetModel(ModelComponent* pModel);

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
	bool IsCharacterStateChanged() { return m_CurrCharacterState != m_PrevCharacterState; }
	XMFLOAT3 GetFootPosition() {
		return m_pControllerComponent->GetFootPosition();} 
	
	CharacterState m_CurrCharacterState{ CharacterState::Idle };
	CharacterState m_PrevCharacterState{ CharacterState::Idle };

private:
	GameObject* m_pModelObj{};

	CameraComponent* m_pCameraComponent{};
	ControllerComponent* m_pControllerComponent{};

	CharacterDesc m_CharacterDesc;
	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{},                              //MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)
		m_CameraHeight{ 2.f },
		m_CameraDist{ 10 },
		m_CurrMeshRot{}, m_RollSpeedUp{0.f};

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)
};

