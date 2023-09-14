#include "stdafx.h"
#include "PongScene.h"
#include "Prefabs/SpherePrefab.h"


PongScene::PongScene()  :

	GameScene(L"PongScene") {}


void PongScene::Initialize()
{
	const auto& CameraTransform = m_SceneContext.pCamera->GetTransform();
	//CAMERA
	//CameraTransform->->Rotate(XMFLOAT3{0.f, 0.f, 0.f });
	CameraTransform->Translate(XMFLOAT3{ 0.f, 30.f, 0.f });



	auto& physx = PxGetPhysics();
	auto pPalletMat = physx.createMaterial(0.f, 0.3f, 5.f);
	auto pBallMat = physx.createMaterial(0.f, 0.3f, 10.f);

	

	//Sphere PONNG BALL
	auto pPongBall = new SpherePrefab(1.0f, 10, XMFLOAT4{ Colors::Red });
	AddChild(pPongBall);

	auto pRigidBodyBall = pPongBall->AddComponent(new RigidBodyComponent(false));
	pRigidBodyBall->AddCollider(PxSphereGeometry{ 1.f }, *pBallMat);
	pRigidBodyBall->SetConstraint(RigidBodyConstraint::AllRot | RigidBodyConstraint::TransY, false);

	pRigidBodyBall->AddForce(XMFLOAT3{ -5.f, 0.f, 0.f }, physx::PxForceMode::eIMPULSE);
	XMFLOAT3 playerDimensions{ 1.f, 2.f, 5.f };

	//PLAYER 1 PALET
	auto pPlayer1 = new CubePrefab(playerDimensions, XMFLOAT4{ Colors::Blue });
	AddChild(pPlayer1);

	m_pPlayer1 = pPlayer1->AddComponent(new RigidBodyComponent(false));
	m_pPlayer1->SetConstraint(RigidBodyConstraint::AllRot | RigidBodyConstraint::TransX | RigidBodyConstraint::TransY, false);
	m_pPlayer1->AddCollider(PxBoxGeometry{ playerDimensions.x / 2.f, playerDimensions.y / 2.f, playerDimensions.z / 2.f }, *pPalletMat);

	m_pPlayer1->Translate(XMFLOAT3{ -10.f, 1.f, 0.f });

	//PLAYER 2 PALET
	auto pPlayer2 = new CubePrefab(playerDimensions, XMFLOAT4{ Colors::Blue });
	AddChild(pPlayer2);

	m_pPlayer2 = pPlayer2->AddComponent(new RigidBodyComponent(false));
	m_pPlayer2->SetConstraint(RigidBodyConstraint::AllRot | RigidBodyConstraint::TransX | RigidBodyConstraint::TransY, false);
	m_pPlayer2->AddCollider(PxBoxGeometry{ playerDimensions.x / 2.f, playerDimensions.y / 2.f, playerDimensions.z / 2.f }, *pPalletMat);

	m_pPlayer2->Translate(XMFLOAT3{ 10.f, 1.f, 0.f });

	//Input
	m_SceneContext.pInput->AddInputAction(InputAction{ InputIds::p1Up,InputState::down,'R' });
	m_SceneContext.pInput->AddInputAction(InputAction{ InputIds::p1Down,InputState::down,'F' });
	m_SceneContext.pInput->AddInputAction(InputAction{ InputIds::p2Up,InputState::down,VK_UP });
	m_SceneContext.pInput->AddInputAction(InputAction{ InputIds::p2Down,InputState::down, VK_DOWN });

	m_SceneContext.settings.clearColor = XMFLOAT4{};

	
	

}

void PongScene::Update()
{
	XMFLOAT3 translationP2{0, 0, 0};
	XMFLOAT3 translationP1{0, 0, 0};
	float speed{10.f};
	//float m_Transportation{};
	if (m_SceneContext.pInput->IsActionTriggered(p2Up))
	{
		translationP2.z += speed * m_SceneContext.pGameTime->GetElapsed();
	}
	else if (m_SceneContext.pInput->IsActionTriggered(p2Down))
	{
		translationP2.z -= speed * m_SceneContext.pGameTime->GetElapsed();
	}

	if(m_SceneContext.pInput->IsActionTriggered(p1Up))
	{
		translationP1.z += speed * m_SceneContext.pGameTime->GetElapsed();
	}
	else if (m_SceneContext.pInput->IsActionTriggered(p1Down))
	{
		translationP1.z -= speed * m_SceneContext.pGameTime->GetElapsed();
	}

	XMStoreFloat3(&translationP2, XMVectorAdd(XMLoadFloat3(&translationP2), XMLoadFloat3(&m_pPlayer2->GetGameObject()->GetTransform()->GetPosition())));
	m_pPlayer2->Translate(XMFLOAT3{ 10.f, 1.f,translationP2.z });

	XMStoreFloat3(&translationP1, XMVectorAdd(XMLoadFloat3(&translationP1), XMLoadFloat3(&m_pPlayer1->GetGameObject()->GetTransform()->GetPosition())));
	m_pPlayer1->Translate(XMFLOAT3{ -10.f, 1.f,translationP1.z });
}


void PongScene::Draw()
{
}

void PongScene::OnGUI()
{
}
