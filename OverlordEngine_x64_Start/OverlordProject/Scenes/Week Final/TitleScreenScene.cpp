#include "stdafx.h"
#include "TitleScreenScene.h"
#include "Materials/ZeldaWaveMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Prefabs/SkyBoxPrefab.h"
#include "TextureUtils.h"
#include "SoundManager/SoundEffectManager.h"

void TitleScreenScene::Initialize()
{
	m_SceneContext.settings.showInfoOverlay = false;
	//Camera
	const auto pCameraObj = AddChild(new GameObject);
	const auto pCamera = pCameraObj->AddChild(new FixedCamera());
	m_pCamComponent = pCamera->GetComponent<CameraComponent>();
	// set as active camera
	m_pCamComponent->SetActive(true);
	m_pCamComponent->GetTransform()->Translate(XMFLOAT3{0.197f, 58.859f, -99.643f});
	m_pCamComponent->GetTransform()->Rotate(XMFLOAT3{ 25.0f, 0.0f, 0.0f });

	m_pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, m_pDefaultMaterial);
	//Water creation
	CreateOceanWaves();

	//Sky Box
	const auto skyBox = new SkyBoxPrefab(L"Textures/SkyBoxToon2.dds");
	skyBox->Initialize(m_SceneContext);
	AddChild(skyBox);
	skyBox->GetTransform()->Scale(0.5f, 0.5f, 0.5f);

	//Create level
	CreateLevel();

	//UI
	auto titleSprite = AddChild(new GameObject());
	titleSprite->GetTransform()->Translate(m_SceneContext.windowWidth * 0.5f, m_SceneContext.windowHeight * 0.5f, .9f);

	m_pGameTitleSprite = titleSprite->AddChild(new GameObject);

	m_pGameTitleSprite->AddComponent(new SpriteComponent(L"Textures/MenuTitle.png", XMFLOAT2(0.5f, 0.5f)));
	m_pGameTitleSprite->GetTransform()->Scale(0.7f, 0.7f, 0.7f);
	m_pGameTitleSprite->GetTransform()->Translate(-10.f, -100.f, 0.f);

	//m_pDaeBannerSprite->GetTransform()->Translate(0.f, 300.f, 0.f);

	auto pressStartSprite = AddChild(new GameObject());
	pressStartSprite->GetTransform()->Translate(m_SceneContext.windowWidth * 0.5f, m_SceneContext.windowHeight * 0.5f, .9f);

	m_pPressStartSprite = pressStartSprite->AddChild(new GameObject);
	m_pPressStartSprite->AddComponent(new SpriteComponent(L"Textures/PressStart.png", XMFLOAT2(0.5f, 0.5f)));
	m_pPressStartSprite->GetTransform()->Translate(0.f, 200.f, 0.f);
	m_pPressStartSprite->GetTransform()->Scale(0.4f, 0.4f, 0.4f);

	auto bannerSprite = AddChild(new GameObject());
	bannerSprite->GetTransform()->Translate(m_SceneContext.windowWidth * 0.5f, m_SceneContext.windowHeight * 0.5f, .9f);

	m_pDaeBannerSprite = bannerSprite->AddChild(new GameObject);
	m_pDaeBannerSprite->AddComponent(new SpriteComponent(L"Textures/DaeBanner.png", XMFLOAT2(0.5f, 0.5f)));

	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.drawGrid = false;

	//Input
	//press start
	auto inputAction = InputAction(StartGame, InputState::pressed, VK_RETURN, -1, XINPUT_GAMEPAD_START);
	m_SceneContext.pInput->AddInputAction(inputAction);
	
	//pres B (back)
	inputAction = InputAction(QuitGame, InputState::down, VK_ESCAPE, -1, XINPUT_GAMEPAD_B);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//Sound effect
	SoundEffectManager::Get()->AddSoundEffect(SoundEffectManager::SoundEffect::PressStart, "Resources/Sounds/SFX/WW_PressStart.wav");
	SoundEffectManager::Get()->AddSoundEffect(SoundEffectManager::SoundEffect::MenuQuit, "Resources/Sounds/SFX/WW_PauseMenu_Quit.wav");
}

void TitleScreenScene::Update()
{
	m_pMusicChannel->setVolume(0.1f);
	//Update waves
	m_pWaveMaterial->Update(GetSceneContext().pGameTime->GetElapsed());

	if (m_SceneContext.pInput->IsActionTriggered(QuitGame))
	{
		if (!m_QuitGame)
		{
			SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::MenuQuit);
			m_QuitGame = true;
		}
	}

	if (m_QuitGame)
	{
		SoundEffectManager::Get()->GetSfxChannel()->isPlaying(&m_IsSfxPlaying);
		m_pMusicChannel->stop();

		if (!m_IsSfxPlaying)
		PostQuitMessage(0);
	}
		
	if (m_SceneContext.pInput->IsActionTriggered(StartGame))
	{
		SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::PressStart);
		m_pMusicChannel->stop();
		SceneManager::Get()->SetActiveGameScene(L"LoadingScreenScene");
	}
}

void TitleScreenScene::OnSceneActivated()
{
	auto pSoundSystem = SoundManager::Get()->GetSystem();
	FMOD::Sound* pSound{};
	pSoundSystem->createStream("Resources/Sounds/Music/TitleTheme.mp3", FMOD_2D, nullptr, &pSound);
	pSound->setMode(FMOD_LOOP_NORMAL);
	pSoundSystem->playSound(pSound, nullptr, false, &m_pMusicChannel);
}

void TitleScreenScene::OnSceneDeactivated()
{
	m_pMusicChannel->stop();
}

void TitleScreenScene::CreateOceanWaves()
{
	//Ground Plane
	m_pWaveMaterial = MaterialManager::Get()->CreateMaterial<ZeldaWaveMaterial>();
	//set wave speed
	m_pWaveMaterial->SetSpeed(1.f);
	//create plane mesh
	const auto pPlaneObject = AddChild(new GameObject());
	const auto pPlaneModel = new ModelComponent(L"Meshes/UnitPlane.ovm");
	pPlaneModel->SetMaterial(m_pWaveMaterial);
	pPlaneObject->AddComponent(pPlaneModel);
	pPlaneObject->GetTransform()->Scale(800.0f, 800.0f, 800.0f);
	pPlaneObject->GetTransform()->Translate(400.f, 0.0f, 0.0f);
}

void TitleScreenScene::CreateLevel()
{
	XMFLOAT3 levelScale{ .01f, .01f, .01f };
	//Simple Level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/WindFallIsland.ovm"));

	//scale ovm
	pLevelMesh->GetTransform()->Scale(levelScale);

	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/WindFallIsland.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ levelScale.x, levelScale.y, levelScale.z })), *m_pDefaultMaterial);

	//scale levelactor
	pLevelActor->GetTransform()->Scale(levelScale);

	pLevelObject->GetTransform()->Scale(levelScale);

	m_IslandPos = pLevelObject->GetTransform()->GetWorldPosition();

	//assigning materials
	std::ifstream objFile(L"Resources/Meshes/Windfall.obj");

	std::ifstream mtlFile(L"Resources/Textures/Windfall.mtl");

	std::wstring folderName(L"");

	//level
	textureUtils::AssignMaterials<DiffuseMaterial_Shadow>(pLevelMesh, objFile, mtlFile, folderName);

}
