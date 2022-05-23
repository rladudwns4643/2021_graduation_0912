#include "pch.h"
#include "SkinnedModelInstance.h"

void SkinnedModelInstance::ChangeSkinnedAnimation(std::string clipName, float timePos)
{
	SkinnedInfo->GetFinalTransforms(clipName, timePos, FinalTransforms);
}

void SkinnedModelInstance::ChangeSkinnedAnimation(std::string prevClipName, float prevTimePos, std::string currClipName, float currTimePos, float factor)
{
	SkinnedInfo->GetBlendedAnimationData(prevClipName, prevTimePos, currClipName, currTimePos, factor, FinalTransforms);
}

