#include "pch.h"
#include "UserInterface.h"
#include "SpriteAnimation.h"

UserInterface::UserInterface(std::string type, std::string id) :
	GameObject(type, id),
	m_Activation(true),
	m_IsSelected(false),
	m_OtherIsSelected(false),
	m_PressedTextureIndex(-1),
	m_NoneActivationTextureIndex(-1),
	m_SelectedTextureIndex(-1)
{
}

UserInterface::~UserInterface()
{
	for (auto& p : m_SpriteAnimations)
		p.second.release();
	m_SpriteAnimations.clear();
}

void UserInterface::InitSpriteAnimation()
{
	m_MaterialIndex = m_OriginMatIndex;

	for (auto& p : m_SpriteAnimations)
	{
		p.second->Initialize();
	}
}

void UserInterface::PlaySpriteAnimation(const float deltaT, std::string spriteName, bool isLoop)
{
	if (!m_SpriteAnimations.count(spriteName)) {
		cout << "ã�� ��������Ʈ �ִϸ��̼��� �������� �ʽ��ϴ�." << endl;
		return;
	}

	if (!isLoop)
		m_SpriteAnimations[spriteName]->PlayAnimation(deltaT);
	else
		m_SpriteAnimations[spriteName]->PlayAnimationLoop(deltaT);
}

void UserInterface::InitializeSyncInfo()
{
	if (!this) return;

	m_Activation = true;
	m_IsSelected = false;
	m_OtherIsSelected = false;

	// ���� �ʱ���·� �ʱ�ȭ
	if (m_ReleasedTextureIndex != -1) {
		m_MaterialIndex = m_ReleasedTextureIndex;
	}
}

int UserInterface::OnSelected()
{
	// Ȱ��ȭ �Ǿ��������� ���� ����
	if (!m_Activation) return -1;
	if (m_OtherIsSelected) return -1;

	// ���õǾ��µ� �� ����
	if (m_IsSelected)
	{
		cout << m_InstID << "�� �ߺ� ���õǾ����ϴ�." << endl;
		m_IsSelected = false;
		return false;
	}

	// ���õǾ����� �ʾҴٸ� ���õǾ��ٰ� ����
	else if (!m_IsSelected)
	{
		cout << m_InstID << "�� ���õǾ����� �ʾҴµ� ���õǾ����ϴ�." << endl;
		m_IsSelected = true;
	}

	return true;
}

void UserInterface::Overlap(bool onoff)
{
	if (!m_Activation) return;

	m_IsPressed = onoff;

	if (m_PressedTextureIndex != -1 && m_IsPressed == true) {
		m_MaterialIndex = m_PressedTextureIndex;
	}
	else if (m_ReleasedTextureIndex != -1 && m_IsPressed == false) {
		m_MaterialIndex = m_ReleasedTextureIndex;
	}

	if (m_SelectedTextureIndex != -1 && (m_IsSelected == true || m_OtherIsSelected == true))
	{
		m_MaterialIndex = m_SelectedTextureIndex;
	}
}

bool UserInterface::Overlap()
{
	if (!m_Activation) return false;

	m_IsPressed = !m_IsPressed;

	if (m_PressedTextureIndex != -1 && m_IsPressed == true) {
		m_MaterialIndex = m_PressedTextureIndex;
	}
	else if (m_ReleasedTextureIndex != -1 && m_IsPressed == false) {
		m_MaterialIndex = m_ReleasedTextureIndex;
	}

	return m_IsPressed;
}

bool UserInterface::GetPressed()
{
	return m_IsPressed;
}

void UserInterface::ForcedUnSelected()
{
	if (!m_Activation) return;

	cout << m_InstID << "�� ���� ���� ��� �Ǿ����ϴ�." << endl;

	m_IsSelected = false;

	// Selected
	if (m_ReleasedTextureIndex != -1)
	{
		m_MaterialIndex = m_ReleasedTextureIndex;
	}
}

void UserInterface::ForcedUnOtherSelected()
{
	m_OtherIsSelected = false;

	// Selected
	if (m_ReleasedTextureIndex != -1)
	{
		m_MaterialIndex = m_ReleasedTextureIndex;
	}
}

void UserInterface::SetOtherIsSelected(bool onoff)
{
	m_OtherIsSelected = onoff;

	if (m_OtherIsSelected)
	{
		// Selected
		if (m_SelectedTextureIndex != -1)
		{
			m_MaterialIndex = m_SelectedTextureIndex;
		}
	}
	else
	{
		// Selected
		if (m_ReleasedTextureIndex != -1)
		{
			m_MaterialIndex = m_ReleasedTextureIndex;
		}
	}
}

bool UserInterface::GetOtherIsSelected() const
{
	return m_OtherIsSelected;
}

void UserInterface::OnActive(bool onoff)
{
	m_Activation = onoff;

	// ��Ȱ��ȭ �ؽ��ķ� ����
	if (m_NoneActivationTextureIndex != -1 && m_Activation == false)
		m_MaterialIndex = m_NoneActivationTextureIndex;
	else if (m_NoneActivationTextureIndex != -1 && m_Activation == true)
		m_MaterialIndex = m_ReleasedTextureIndex;
}

void UserInterface::SetReleasedTexture(int texIndex)
{
	m_ReleasedTextureIndex = texIndex;
}

void UserInterface::SetPressedTexture(int texIndex)
{
	m_PressedTextureIndex = texIndex;
}

void UserInterface::SetDisabledTexture(int texIndex)
{
	m_NoneActivationTextureIndex = texIndex;
}

void UserInterface::SetSelectedTexture(int texIndex)
{
	m_SelectedTextureIndex = texIndex;
}

void UserInterface::SetSpriteAniamtion(std::string spriteName, float durationTime, float animationSpeed)
{
	if (m_SpriteAnimations.count(spriteName)) {
		cout << "��������Ʈ �ִϸ��̼� �̸��� �ߺ��Ǿ����ϴ�." << endl;
		return;
	}

	m_SpriteAnimations[spriteName] = std::make_unique<SpriteAnimation>(this, durationTime, animationSpeed);
}

SpriteAnimation* UserInterface::GetSpriteAnimation(std::string spriteName)
{
	if (!m_SpriteAnimations.count(spriteName)) {
		cout << "ã�� ��������Ʈ �ִϸ��̼��� �������� �ʽ��ϴ�." << endl;
		return nullptr;
	}

	return m_SpriteAnimations[spriteName].get();
}

void UserInterface::AddFrame(std::string spriteName, int matIndex)
{
	if (!m_SpriteAnimations.count(spriteName)) {
		cout << "ã�� ��������Ʈ �ִϸ��̼��� �������� �ʽ��ϴ�." << endl;
		return;
	}

	m_SpriteAnimations[spriteName]->AddFrame(matIndex);
}

void UserInterface::SetDurationTime(std::string spriteName, float durationTime)
{
	if (!m_SpriteAnimations.count(spriteName)) {
		cout << "ã�� ��������Ʈ �ִϸ��̼��� �������� �ʽ��ϴ�." << endl;
		return;
	}

	m_SpriteAnimations[spriteName]->SetDurationTime(durationTime);
}

void UserInterface::SetTextAlignType(TextAlignType anchorType)
{
	m_TextAlignType = anchorType;
}

TextAlignType UserInterface::GetTextAlignType() const
{
	return m_TextAlignType;
}

void UserInterface::SetOriginMaterial(int originMatIndex)
{
	m_OriginMatIndex = originMatIndex;
}

int UserInterface::GetOriginMaterial() const
{
	return m_OriginMatIndex;
}
