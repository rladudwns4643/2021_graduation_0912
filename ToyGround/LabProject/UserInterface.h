#pragma once
#include "GameObject.h"

class SpriteAnimation;
class UserInterface : public GameObject
{
public:
	std::string m_UIName;

	bool m_IsText = false;
	XMFLOAT2 m_UIPos;
	XMFLOAT2 m_UISize;

public:
	explicit UserInterface(std::string type, std::string id);
	virtual ~UserInterface();

	void InitSpriteAnimation();
	void PlaySpriteAnimation(const float deltaT, std::string spriteName, bool isLoop);

public:
	void InitializeSyncInfo();

	void OnActive(bool onoff);
	int	OnSelected();
	void Overlap(bool onoff);
	bool Overlap();

	bool GetPressed();

	void ForcedUnSelected();
	void ForcedUnOtherSelected();

	void SetOtherIsSelected(bool onoff);
	bool GetOtherIsSelected() const;

public:
	void SetReleasedTexture(int texIndex);
	void SetPressedTexture(int texIndex);
	void SetDisabledTexture(int texIndex);
	void SetSelectedTexture(int texIndex);

public:
	// SetSpriteAnimation
	void				SetSpriteAniamtion(std::string spriteName, float durationTime, float animationSpeed = 1.f);
	SpriteAnimation* GetSpriteAnimation(std::string spriteName);
	void				AddFrame(std::string spriteName, int matIndex);
	void				SetDurationTime(std::string spriteName, float durationTime);

	void				SetTextAlignType(TextAlignType TextAlignType);
	TextAlignType			GetTextAlignType() const;

	// ��������Ʈ �ִϸ��̼��� �۵��� ���� �̹����� ���ư��� ���� �Ķ���� ����
	void				SetOriginMaterial(int originMatIndex);
	int					GetOriginMaterial() const;

protected:
	std::map<std::string, std::unique_ptr<SpriteAnimation>> m_SpriteAnimations;

	int			m_OriginMatIndex = 0;
	TextAlignType	m_TextAlignType;

protected:
	// ��ư�� ������ �ȴ����� : false
	bool m_IsPressed;

	// ��ư ���õ� / ���õ������� : false
	bool m_IsSelected;
	bool m_OtherIsSelected;

	// ��ư Ȱ��ȭ / ��Ȱ��ȭ : true
	bool m_Activation;

protected:
	// Active(Released): ���� �ؽ��� �ε���
	// Pressed: ��������
	// Disable: ��Ȱ��ȭ
	int m_ReleasedTextureIndex;
	int m_PressedTextureIndex;
	int m_NoneActivationTextureIndex;
	int m_SelectedTextureIndex;
};

