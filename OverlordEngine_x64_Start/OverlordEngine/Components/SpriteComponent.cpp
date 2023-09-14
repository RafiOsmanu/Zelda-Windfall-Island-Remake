#include "stdafx.h"
#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(const std::wstring& spriteAsset, const XMFLOAT2& pivot, const XMFLOAT4& color):
	m_SpriteAsset(spriteAsset),
	m_Pivot(pivot),
	m_Color(color)
{}

void SpriteComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
	m_SpriteAsset = spriteAsset;
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::Draw(const SceneContext& /*sceneContext*/)
{
	if (!m_pTexture)
		return;

	// Get the transform component
	auto pTransform = m_pGameObject->GetComponent<TransformComponent>();
 
	SpriteRenderer::Get()->AppendSprite(
		m_pTexture,
		XMFLOAT2(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().y),
		m_Color,
		m_Pivot,
		XMFLOAT2(pTransform->GetScale().x, pTransform->GetScale().y),
		MathHelper::QuaternionToEuler(pTransform->GetRotation()).z,
		pTransform->GetPosition().z);
}


