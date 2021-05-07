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

	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
};

