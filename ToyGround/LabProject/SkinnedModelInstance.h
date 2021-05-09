#pragma once
#include "SkinnedData.h"

class SkinnedModelInstance
{
public:
	void ChangeSkinnedAnimation(std::string clipName, float timePos);
	void ChangeSkinnedAnimation(std::string prevClipName, float prevTimePos, std::string currClipName, float currTimePos, float factor);

public:
	// meshName / skinnedData
	std::unique_ptr<SkinnedData> SkinnedInfo = nullptr;

	// 주어진 시간에서의 최종 변환들을 담는다.
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
};

