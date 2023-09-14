#include "stdafx.h"
#include <codecvt>
#include <string>
#include "WindwakerScene.h"

#include "Prefabs/Character.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/ZeldaWaveMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Prefabs/SkyBoxPrefab.h"
#include "Prefabs/Link.h"
#include "TextureUtils.h"
#include "Prefabs/Barrel.h"
#include "SoundManager/SoundEffectManager.h"
#include "Materials/Post/PostDim.h"


void WindwakerScene::Initialize()
{
	m_SceneContext.settings.showInfoOverlay = false;
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	//Ground Plane + material 
	m_pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, m_pDefaultMaterial);
	
	//Create Ocean Wave Plane
	CreateOceanWaves();

	//Inititalize Link
	CreateCharacter();
	
	//create level island
	CreateLevel();

	//Initialize input
	SetUpInput();

	//create barrels
	CreateBarrels();

	//setup UI
	SetupUI();
	

	//Sky Box
	const auto skyBox = new SkyBoxPrefab(L"Textures/SkyBoxToon2.dds");
	skyBox->Initialize(m_SceneContext);
	AddChild(skyBox);
	skyBox->GetTransform()->Scale(0.5f, 0.5f, 0.5f);
	
	m_SceneContext.settings.drawPhysXDebug = false;

	//Sound
	SoundEffectManager::Get()->AddSoundEffect(SoundEffectManager::SoundEffect::GameWon, "Resources/Sounds/SFX/WW_Fanfare_GameWon.wav");
	SoundEffectManager::Get()->AddSoundEffect(SoundEffectManager::SoundEffect::MenuOpen, "Resources/Sounds/SFX/WW_PauseMenu_Open.wav");
	SoundEffectManager::Get()->AddSoundEffect(SoundEffectManager::SoundEffect::MenuClose, "Resources/Sounds/SFX/WW_PauseMenu_Close.wav");
	SoundEffectManager::Get()->AddSoundEffect(SoundEffectManager::SoundEffect::MenuQuit, "Resources/Sounds/SFX/WW_PauseMenu_Quit.wav");
	SoundEffectManager::Get()->AddSoundEffect(SoundEffectManager::SoundEffect::MenuCursor, "Resources/Sounds/SFX/WW_PauseMenu_Cursor.wav");
	SoundEffectManager::Get()->AddSoundEffect(SoundEffectManager::SoundEffect::MenuSelect, "Resources/Sounds/SFX/WW_PauseMenu_Select.wav");

	//PostProcessing
	m_pPostDim = MaterialManager::Get()->CreateMaterial<PostDim>();
	AddPostProcessingEffect(m_pPostDim);
	m_pPostDim->SetDimming(1.f);

}

void WindwakerScene::OnGUI()
{
	m_pLink->DrawImGui();
	
}

void WindwakerScene::Update()
{
	if (m_pContinueBoxSprite->GetIsEnabled())
	{
		m_pMusicChannel->setVolume(0.05f);
	}
	else
	{
		m_pMusicChannel->setVolume(0.1f);
	}
	
	//Update waves
	if(!m_pQuitBoxSprite->GetIsEnabled())
	m_pWaveMaterial->Update(GetSceneContext().pGameTime->GetElapsed());

	//update light pos

	const auto lightDir = m_SceneContext.pLights->GetDirectionalLight().direction;
	XMFLOAT3 lightDirection = XMFLOAT3{ lightDir.x, lightDir.y, lightDir.z };

	XMFLOAT3 posOnIsland{ m_pLink->GetTransform()->GetWorldPosition() };
	XMFLOAT3 lightPos{ (-lightDirection.x * m_LightDistance) + posOnIsland.x, (-lightDirection.y * m_LightDistance) + posOnIsland.y, (-lightDirection.z * m_LightDistance) + posOnIsland.z };
	m_SceneContext.pLights->SetDirectionalLight(lightPos, lightDirection);

	std::string text = std::to_string(m_pLink->GetAmountOfBarrelsDestroyed()) + "/" + std::to_string(m_pLink->GetAmountOfBarrels());

	//UI
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(text), m_TextPos, m_TextColor);

	if (m_pLink->GameWon() && !m_GameWonPlayed)
	{
		SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::GameWon);
		m_TextColor = DirectX::XMFLOAT4{ 1.f, 244.f / 255.f, 0, 1.f };
		m_GameWonPlayed = true;
	}

	if (m_SceneContext.pInput->IsActionTriggered(MenuPauze))
	{
		PauzeGame();

	}
	else if (m_SceneContext.pInput->IsActionTriggered(MenuRight))
	{
		++m_InPutId;
		m_InPutId = m_InPutId % 3;

		if (m_InPutId <= 0) m_InPutId = 1;
		SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::MenuCursor);

	}
	else if (m_SceneContext.pInput->IsActionTriggered(MenuLeft))
	{
		--m_InPutId;
		if (m_InPutId <= 0) m_InPutId = 2;
		SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::MenuCursor);

	}

	CheckInputId();

	//disable controlls panel
	if (m_pControllsSprite->GetIsEnabled())
	{
		if (m_SceneContext.pInput->IsActionTriggered(CharacterJump) || m_SceneContext.pInput->IsActionTriggered(CharacterRoll))
		{
			m_pControllsSprite->SetIsEnabled(false);
		}
	}
	

	
}

void WindwakerScene::PostDraw()
{
	//Draw ShadowMap (Debug Visualization)
	if (m_DrawShadowMap) {

		ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { m_ShadowMapScale, m_ShadowMapScale }, { 1.f,0.f });
	}
}

void WindwakerScene::OnSceneActivated()
{
	auto pSoundSystem = SoundManager::Get()->GetSystem();
	FMOD::Sound* pSound{};
	pSoundSystem->createStream("Resources/Sounds/Music/WindFallIslandTheme.mp3", FMOD_2D, nullptr, &pSound);
	pSound->setMode(FMOD_LOOP_NORMAL);
	pSoundSystem->playSound(pSound, nullptr, false, &m_pMusicChannel);
}

void WindwakerScene::OnSceneDeactivated()
{
	m_pMusicChannel->stop();
}

void WindwakerScene::CreateOceanWaves()
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

void WindwakerScene::CreateCharacter()
{
	//Character
	CharacterDesc characterDesc{ m_pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	characterDesc.actionId_Roll = CharacterRoll;

	m_pLink = AddChild(new Link(characterDesc));
	m_pLink->GetTransform()->Translate(-4.873f, 3.5f, -20.616f);
	
	
}

void WindwakerScene::CreateLevel()
{
	XMFLOAT3 levelScale{ .01f, .01f, .01f };
	//Simple Level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/WindFallIsland.ovm"));
	pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());

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

void WindwakerScene::SetUpInput()
{
	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterRoll, InputState::pressed, VK_LSHIFT, -1, XINPUT_GAMEPAD_B);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(MenuPauze, InputState::pressed, 'P', -1, XINPUT_GAMEPAD_START);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(MenuRight, InputState::pressed, VK_RIGHT, -1, XINPUT_GAMEPAD_DPAD_RIGHT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(MenuLeft, InputState::pressed, VK_LEFT, -1, XINPUT_GAMEPAD_DPAD_LEFT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(MenuSelect, InputState::pressed, VK_LEFT, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);


}

void WindwakerScene::CreateBarrels()
{
	m_pBarrel = AddChild(new GameObject());

	auto pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(-2.9f, 10.4f, 2.829f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(14.9f, 9.4f, -11.871f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(-32.881f, 6.219f, -20.495f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(-47.081f, 6.919f, -19.995f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(-16.981f, 3.219f, 4.105f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(-29.781f, 11.319f, 34.605f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(7.419f, 10.219f, 50.305f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(17.019f, 6.719f, 43.305f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(38.819f, 0.319f, 21.005f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(32.119f, 4.419f, 12.205f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(19.819f, 8.119f, 18.005f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(19.119f, 15.919f, 13.305f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(-7.181f, 10.419f, 21.905f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(-2.081f, 14.219f, 4.105f);
	pBarrel = m_pBarrel->AddChild(new Barrel(m_pLink));
	pBarrel->GetTransform()->Translate(9.524f, 1.f, -31.821f);
}

void WindwakerScene::SetupUI()
{
	auto UI = AddChild(new GameObject());
	UI->GetTransform()->Translate(m_SceneContext.windowWidth * 0.5f, m_SceneContext.windowHeight * 0.5f, .9f);

	m_pBarrelSprite = UI->AddChild(new GameObject);

	m_pBarrelSprite->AddComponent(new SpriteComponent(L"Textures/BarrelIcon.png", XMFLOAT2(0.5f, 0.5f)));
	m_pBarrelSprite->GetTransform()->Translate(m_SceneContext.windowWidth * 0.3f, m_SceneContext.windowHeight * 0.38f, .9f);
	m_pBarrelSprite->GetTransform()->Scale(0.2f, 0.2f, 0.5f);

	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/WindWaker_64.fnt");

	
	//Quit
	auto quitBox = AddChild(new GameObject());
	quitBox->GetTransform()->Translate(m_SceneContext.windowWidth * 0.5f, m_SceneContext.windowHeight * 0.5f, .9f);

	m_pQuitBoxSprite = quitBox->AddChild(new GameObject);
	m_pQuitBoxSprite->AddComponent(new SpriteComponent(L"Textures/Quit.png", XMFLOAT2(0.5f, 0.5f)));
	m_pQuitBoxSprite->GetTransform()->Scale(0.3f, 0.3f, 0.3f);
	m_pQuitBoxSprite->GetTransform()->Translate(200.f, 0.f, 0.f);
	m_pQuitBoxSprite->SetIsEnabled(false);

	//Continue
	auto continueBox = AddChild(new GameObject());
	continueBox->GetTransform()->Translate(m_SceneContext.windowWidth * 0.5f, m_SceneContext.windowHeight * 0.5f, .9f);

	m_pContinueBoxSprite = continueBox->AddChild(new GameObject);
	m_pContinueBoxSprite->AddComponent(new SpriteComponent(L"Textures/Continue.png", XMFLOAT2(0.5f, 0.5f)));
	m_pContinueBoxSprite->GetTransform()->Scale(0.3f, 0.3f, 0.3f);
	m_pContinueBoxSprite->GetTransform()->Translate(-200.f, 0.f, 0.f);
	m_pContinueBoxSprite->SetIsEnabled(false);

	//Controlls
	auto controlls = AddChild(new GameObject());
	controlls->GetTransform()->Translate(m_SceneContext.windowWidth * 0.5f, m_SceneContext.windowHeight * 0.5f, .9f);

	m_pControllsSprite = controlls->AddChild(new GameObject);
	m_pControllsSprite->AddComponent(new SpriteComponent(L"Textures/Controlls.png", XMFLOAT2(0.5f, 0.5f)));
	m_pControllsSprite->GetTransform()->Scale(0.8f, 0.8f, 0.8f);
	//m_pControllsSprite->GetTransform()->Translate(-200.f, 0.f, 0.f);
	m_pControllsSprite->SetIsEnabled(true);
	
	
}

void WindwakerScene::PauzeGame()
{
	if (!m_pQuitBoxSprite->GetIsEnabled())
	{
		//m_pPauzeBoxSprite->SetIsEnabled(true);
		m_pQuitBoxSprite->SetIsEnabled(true);
		m_pContinueBoxSprite->SetIsEnabled(true);
		m_pLink->SetIsPaused(true);
		m_pPostDim->SetDimming(0.5f);
		SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::MenuOpen);
	}
	else
	{
		//m_pPauzeBoxSprite->SetIsEnabled(false);
		m_pQuitBoxSprite->SetIsEnabled(false);
		m_pContinueBoxSprite->SetIsEnabled(false);
		m_pLink->SetIsPaused(false);
		m_pPostDim->SetDimming(1.f);
		SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::MenuClose);

	}
	
}

void WindwakerScene::CheckInputId()
{
	if (m_InPutId == 1)
	{
		m_pQuitBoxSprite->GetTransform()->Scale({ 0.4f, 0.4f, 0.4f });
		m_pContinueBoxSprite->GetTransform()->Scale({ 0.3f, 0.3f, 0.3f });
		//used character jump as confirm button A
		if (m_SceneContext.pInput->IsActionTriggered(MenuSelect) && m_pQuitBoxSprite->GetIsEnabled())
		{
			if (!m_QuitGame)
			{
				SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::MenuSelect);
				SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::MenuQuit);
				m_QuitGame = true;
			}
		}
	}
	else if (m_InPutId == 2)
	{
		m_pContinueBoxSprite->GetTransform()->Scale({ 0.4f, 0.4f, 0.4f });
		m_pQuitBoxSprite->GetTransform()->Scale({ 0.3f, 0.3f, 0.3f });
		//used character jump as confirm button A
		if (m_SceneContext.pInput->IsActionTriggered(MenuSelect) && m_pQuitBoxSprite->GetIsEnabled())
		{
			SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::MenuSelect);

			SoundEffectManager::Get()->PlaySoundEffect(SoundEffectManager::SoundEffect::MenuClose);

			PauzeGame();
		}
	}

	if (m_QuitGame)
	{
		SoundEffectManager::Get()->GetSfxChannel()->isPlaying(&m_IsSfxPlaying);
		m_pMusicChannel->stop();
		if (!m_IsSfxPlaying)
		{
			m_QuitGame = false;
			//PostQuitMessage(0);
			SceneManager::Get()->SetActiveGameScene(L"TitleScreenScene");
		}
	}


}








