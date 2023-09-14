#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"
#include "Managers/MaterialManager.h"
#include "Managers/ContentManager.h"
#include "Utils/VertexHelper.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	delete[] m_ParticlesArray;
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	//INITIALIZE

	if (m_pParticleMaterial == nullptr) m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();

	CreateVertexBuffer(sceneContext);

	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	if(m_pVertexBuffer)
		SafeRelease(m_pVertexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc{};
	

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = m_ParticleCount * sizeof(VertexParticle);
	vertexBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_FLAG::D3D10_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	
	SUCCEEDED(sceneContext.d3dContext.pDevice->CreateBuffer(&vertexBufferDesc, nullptr, &m_pVertexBuffer));
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	float particleInterval = (m_EmitterSettings.maxEnergy - m_EmitterSettings.minEnergy) / m_ParticleCount;

	float elapsedTime = sceneContext.pGameTime->GetElapsed();

	m_LastParticleSpawn += elapsedTime;

	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource{};

	

	SUCCEEDED(sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	VertexParticle* pBuffer = static_cast<VertexParticle*>(mappedResource.pData);

	
	for (UINT i{}; i < m_ParticleCount; ++i)
	{

		if (m_ParticlesArray[i].isActive)
		{
			UpdateParticle(m_ParticlesArray[i], elapsedTime);
		}
		if (!m_ParticlesArray[i].isActive && m_LastParticleSpawn >= particleInterval && m_IsEnabled)
		{
			SpawnParticle(m_ParticlesArray[i]);
			m_LastParticleSpawn = 0;
		}

		if (m_ParticlesArray[i].isActive)
		{
			pBuffer[m_ActiveParticles] = m_ParticlesArray[i].vertexInfo;
			++m_ActiveParticles;
		}
		
	}

	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
	
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	TODO_W9(L"Implement UpdateParticle")
		if (!p.isActive) return;

	//deactivate particle if energy is below 0
	if ((p.currentEnergy -= elapsedTime) < 0)
	{
		p.isActive = false;
		return;
	}

	
	//add emitter velocity multiplied by elapsed time to the position of the particle
	auto particleVelocity = XMVectorScale(XMLoadFloat3(&m_EmitterSettings.velocity), elapsedTime);

	XMFLOAT3 velocity{};
	XMStoreFloat3(&velocity, particleVelocity);
	p.vertexInfo.Position.x += velocity.x;
	p.vertexInfo.Position.y += velocity.y;
	p.vertexInfo.Position.z += velocity.z;

	float lifePercentage = p.currentEnergy / p.totalEnergy;

	p.vertexInfo.Color = m_EmitterSettings.color;
	p.vertexInfo.Color.w *= lifePercentage * 2;
	
	p.vertexInfo.Size = p.initialSize * (1.f + (p.sizeChange - 1.f) * (1.f - lifePercentage));
	
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	TODO_W9(L"Implement SpawnParticle")
	p.isActive = true;

	//Energy Initialization
	p.currentEnergy = p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);

	//Position Initialization 
	XMVECTOR randomDirection = { 1, 0, 0 };
	XMMATRIX randomRotation = XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI));

	randomDirection = XMVector3TransformNormal(randomDirection, randomRotation);

	float distance = MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius);
	XMVECTOR transformPos = DirectX::XMLoadFloat3(&GetTransform()->GetPosition());

	XMFLOAT3 initialPosition{};

	XMStoreFloat3(&initialPosition, transformPos + XMVectorScale(randomDirection, distance));

	p.vertexInfo.Position = initialPosition;

	//Size Initialization
	p.initialSize = p.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	//Rotation Initialization
	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	//Color Initialization
	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement PostDraw")
	//Set the particle texture and the world view projection matrix for the particle shader
	auto pCamera = sceneContext.pCamera;
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture->GetShaderResourceView());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", pCamera->GetViewInverse());

	//Get the technique from the material and set it as the active one
	auto pTechniqueContext = m_pParticleMaterial->GetTechniqueContext();
	auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;

	//Set Inputlayout
	pDeviceContext->IASetInputLayout(pTechniqueContext.pInputLayout);

	//Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//Set Vertex Buffer
	const UINT offset = 0;
	const UINT vertexStride = sizeof(VertexParticle);
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &vertexStride,
		&offset);

	//DRAW
	auto tech = pTechniqueContext.pTechnique;
	D3DX11_TECHNIQUE_DESC techDesc{};

	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_ActiveParticles, 0);
	}

	
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}