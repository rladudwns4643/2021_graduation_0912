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

	// 스프라이트 애니메이션이 작동후 원래 이미지로 돌아가기 위한 파라미터 세팅
	void				SetOriginMaterial(int originMatIndex);
	int					GetOriginMaterial() const;

protected:
	std::map<std::string, std::unique_ptr<SpriteAnimation>> m_SpriteAnimations;

	int			m_OriginMatIndex = 0;
	TextAlignType	m_TextAlignType;

protected:
	// 버튼이 눌린지 안눌린지 : false
	bool m_IsPressed;

	// 버튼 선택됨 / 선택되지않음 : false
	bool m_IsSelected;
	bool m_OtherIsSelected;

	// 버튼 활성화 / 비활성화 : true
	bool m_Activation;

protected:
	// Active(Released): 기존 텍스쳐 인덱스
	// Pressed: 눌렸을때
	// Disable: 비활성화
	int m_ReleasedTextureIndex;
	int m_PressedTextureIndex;
	int m_NoneActivationTextureIndex;
	int m_SelectedTextureIndex;
};

