#include "stdafx.h"
#include "Character.h"


Character::Character(const CharacterDesc& characterDesc) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	m_pModelObj = AddChild(new GameObject());
	
	

	//Camera
	const auto pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = pCamera->GetComponent<CameraComponent>();
	m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera

	pCamera->GetTransform()->Translate(0.f, m_CharacterDesc.controller.height * .5f, -10.f);

	
}

void Character::Update(const SceneContext& sceneContext)
{
	if (m_pCameraComponent->IsActive())
	{
		m_PrevCharacterState = m_CurrCharacterState;
		constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero

		//***************
		//HANDLE INPUT

		//## Input Gathering (move)
		XMFLOAT2 move{}; 
		//move.y should contain a 1 (Forward) or -1 (Backward) based on the active input (check corresponding actionId in m_CharacterDesc)

		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward))
		{
			move.y = 1;
			std::cout << "move forward" << "\n";
		}
		else if(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward))
		{
			move.y = -1;
			std::cout << "move back" << "\n";
		}
		else
		{
			move.y = 0;
		}

		//Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement
		if (abs(move.y) < epsilon && m_CurrCharacterState != CharacterState::Roll)
		{
			move.y = InputManager::GetThumbstickPosition(true).y;
		}
		//move.x should contain a 1 (Right) or -1 (Left) based on the active input (check corresponding actionId in m_CharacterDesc)

		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
		{
			move.x = 1;
			std::cout << "move right" << "\n";
		}
		else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
		{
			move.x = -1;
			std::cout << "move left" << "\n";
		}
		else
		{
			move.x = 0;
		}
		//Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement
		if (abs(move.x) < epsilon && m_CurrCharacterState != CharacterState::Roll)
		{
			move.x = InputManager::GetThumbstickPosition(true).x;
		}


		//## Input Gathering (look)
		XMFLOAT2 look{ 0.f, 0.f }; //Uncomment
		//Only if the Left Mouse Button is Down 
		if (InputManager::IsMouseButton(InputState::down, VK_LBUTTON))
		{
			// Store the MouseMovement in the local 'look' variable (cast is required)
			look.x = static_cast<float>(sceneContext.pInput->GetMouseMovement().x);
			look.y = static_cast<float>(sceneContext.pInput->GetMouseMovement().y);
			std::cout << "mousePos -> x: " << look.x << ", y: " << look.y << "\n";
		}
		//Optional: in case look.x AND look.y are near zero, you could use the Right ThumbStickPosition for look
		if ((abs(look.x) < epsilon) && (abs(look.y) < epsilon))
		{
			look.x = (float)InputManager::GetThumbstickPosition(false).x;
			look.y = (float)InputManager::GetThumbstickPosition(false).y;
		}
	
		//************************
		//GATHERING TRANSFORM INFO

		//Retrieve the TransformComponent
		
		//***************
		//CAMERA ROTATION

		//Adjust the TotalYaw (m_TotalYaw) & TotalPitch (m_TotalPitch) based on the local 'look' variable
		//Make sure this calculated on a framerate independent way and uses CharacterDesc::rotationSpeed.

		float elapsedTime = sceneContext.pGameTime->GetElapsed();
		float yawSpeedModifier = 0.04f;

		m_TotalYaw += look.x * m_CharacterDesc.rotationSpeed * elapsedTime * yawSpeedModifier;
		m_TotalPitch += look.y * m_CharacterDesc.rotationSpeed * elapsedTime;
		m_CameraHeight -= look.y * elapsedTime * m_CharacterDesc.rotationSpeed * 0.13f;
		m_CameraDist -= look.y * elapsedTime * m_CharacterDesc.rotationSpeed * 0.25f;

		// limit the camera height and distance to the player
		m_CameraHeight = std::max(std::min(m_CameraHeight, m_CharacterDesc.maxCameraHeight), m_CharacterDesc.minCameraHeight);
		m_CameraDist = std::max(std::min(m_CameraDist, m_CharacterDesc.maxCameraDist), m_CharacterDesc.minCameraDist);


		// translate the camera in a circular way around the player
		XMFLOAT3 newCameraPos{};
		newCameraPos.x = -m_CameraDist * sinf(m_TotalYaw);
		newCameraPos.y = m_CameraHeight;
		newCameraPos.z = -m_CameraDist * cosf(m_TotalYaw);
		m_pCameraComponent->GetTransform()->Translate(newCameraPos);

		// always look at the player
		XMFLOAT3 center = XMFLOAT3(0, 0, 0);
		XMVECTOR cameraTarget = XMLoadFloat3(&center);
		XMVECTOR cameraPos = XMLoadFloat3(&m_pCameraComponent->GetTransform()->GetPosition());

		XMVECTOR cameraDir = XMVectorSubtract(cameraTarget, cameraPos);


		// store the normalized camera direction vector
		XMFLOAT3 cameraDirNormalized{};
		XMStoreFloat3(&cameraDirNormalized, (cameraDir));


		float pitch = atan2f(sqrtf(cameraDirNormalized.z * cameraDirNormalized.z + cameraDirNormalized.x * cameraDirNormalized.x), cameraDirNormalized.y) - DirectX::XM_PIDIV2;

		float yaw = -atan2f(cameraDirNormalized.z, cameraDirNormalized.x) + DirectX::XM_PIDIV2;

		m_pCameraComponent->GetTransform()->Rotate(pitch, yaw, 0, false);

		XMFLOAT3 CameraForward = (m_pCameraComponent->GetTransform()->GetForward());
		CameraForward.y = 0.f;

		XMFLOAT3 CameraRight = (m_pCameraComponent->GetTransform()->GetRight());
		CameraRight.y = 0.f;
		XMVECTOR forward = XMLoadFloat3(&CameraForward);
		XMVECTOR right = XMLoadFloat3(&CameraRight);
		
		//********
		//MOVEMENT

		//roll
		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Roll))
		{
			m_CurrCharacterState = CharacterState::Roll;
		}

		//## Horizontal Velocity (Forward/Backward/Right/Left)
		//Calculate the current move acceleration for this frame (m_MoveAcceleration * ElapsedTime)
		//If the character is moving (= input is pressed)
		if (move.x != 0 || move.y != 0)
		{

			if (m_CurrCharacterState != CharacterState::Roll)
			{
				m_CurrCharacterState = CharacterState::Run;
				m_RollSpeedUp = 0.f;
			}
			else
			{
				m_RollSpeedUp = 0.65f;
			}
			
			//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
			XMVECTOR currentdir = XMVector3Normalize(forward * move.y + right * move.x);
			XMStoreFloat3(&m_CurrentDirection, currentdir);
			
			if (m_MoveSpeed <= m_CharacterDesc.maxMoveSpeed)
			{
				m_MoveSpeed += m_MoveAcceleration * elapsedTime;
			}


			//mesh rotation
			float meshYaw = -atan2f(m_CurrentDirection.z, m_CurrentDirection.x) - DirectX::XM_PIDIV2;

			//if the difference between the current mesh rotation and the calculated model yaw is greater than pi radians. 
			//the mesh is rotated in the opposite direction to minimize the rotation distance.
			if (abs(meshYaw - m_CurrMeshRot) > DirectX::XM_PI)
			{
				if (meshYaw < 0)
					m_CurrMeshRot = std::lerp(m_CurrMeshRot, meshYaw + DirectX::XM_PI * 2.f, 0.5f);
				else
				{
					m_CurrMeshRot = std::lerp(m_CurrMeshRot, meshYaw - DirectX::XM_PI * 2.f, 0.5f);
				}
			}
			else
			{
				m_CurrMeshRot = std::lerp(m_CurrMeshRot, meshYaw, 0.5f);
			}

			if (m_CurrMeshRot > DirectX::XM_PIDIV2)
				m_CurrMeshRot -= DirectX::XM_PI * 2.f;
			if (m_CurrMeshRot < -DirectX::XM_PIDIV2)
				m_CurrMeshRot += DirectX::XM_PI * 2.f;

			m_pModelObj->GetTransform()->Rotate(0, m_CurrMeshRot, 0, false);
			
		}
		else
		{
		//Else (character is not moving, or stopped moving)
		
			if (m_CurrCharacterState != CharacterState::Roll)
			{
				m_CurrCharacterState = CharacterState::Idle;
				m_RollSpeedUp = 0.f;
			}
			else
			{
				m_RollSpeedUp = .3f;
			}

			if (m_MoveSpeed > 0)
			{
				m_MoveSpeed -= m_MoveAcceleration * elapsedTime;
			}
			else
			{
				m_MoveSpeed = 0;
			}
		}

		//Now we can calculate the Horizontal Velocity which should be stored in m_TotalVelocity.xz
		//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)
		//Set the x/z component of m_TotalVelocity (horizontal_velocity x/z)
		//It's important that you don't overwrite the y component of m_TotalVelocity (contains the vertical velocity)
		m_TotalVelocity.x = (m_CurrentDirection.x * m_MoveSpeed * elapsedTime) + (m_CurrentDirection.x * m_RollSpeedUp);
		m_TotalVelocity.z = (m_CurrentDirection.z * m_MoveSpeed * elapsedTime) + (m_CurrentDirection.z * m_RollSpeedUp);

		//## Vertical Movement (Jump/Fall)
		//If the Controller Component is NOT grounded (= freefall)
		if (!m_pControllerComponent->GetCollisionFlags().isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))
		{
			if (m_PrevCharacterState != CharacterState::Jump)
				m_CurrCharacterState = CharacterState::Falling;
			else
			{
				m_CurrCharacterState = CharacterState::Jump;
			}
			
			//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
			//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
			if(m_TotalVelocity.y > -m_CharacterDesc.maxFallSpeed)
			m_TotalVelocity.y -= m_FallAcceleration * elapsedTime;
		}
		else if(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump))
		{
			m_CurrCharacterState = CharacterState::Jump;
			//Else If the jump action is triggered
			//Set m_TotalVelocity.y equal to CharacterDesc::JumpSpeed
			m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
		}
		else
		{
			//Else (=Character is grounded, no input pressed)
			//m_TotalVelocity.y is zero
			m_TotalVelocity.y = -0.1f;
		}

		//************
		//DISPLACEMENT

		//The displacement required to move the Character Controller (ControllerComponent::Move) can be calculated using our TotalVelocity (m/s)
		//Calculate the displacement (m) for the current frame and move the ControllerComponent
		/*XMVECTOR totalVel = XMLoadFloat3(&m_TotalVelocity);
		XMVectorScale(totalVel, elapsedTime);
		XMStoreFloat3(&m_TotalVelocity, totalVel);*/
		m_pControllerComponent->Move(m_TotalVelocity);

		//The above is a simple implementation of Movement Dynamics, adjust the code to further improve the movement logic and behaviour.
		//Also, it can be usefull to use a seperate RayCast to check if the character is grounded (more responsive)
	}
}

ModelComponent* Character::SetModel(ModelComponent* pModel)
{
	m_pModelObj->AddComponent(pModel);
	return pModel;
}

void Character::DrawImGui()
{
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_CharacterDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_CharacterDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_CharacterDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_CharacterDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_CharacterDesc.maxMoveSpeed / m_CharacterDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_CharacterDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_CharacterDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_CharacterDesc.maxFallSpeed / m_CharacterDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_CharacterDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_CharacterDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");

		bool isActive = m_pCameraComponent->IsActive();
		if(ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}