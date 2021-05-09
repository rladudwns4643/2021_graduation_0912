#include "pch.h"
#include "SkinnedData.h"

Keyframe::Keyframe()
	: TimePos(0.0f),
	Translation(0.0f, 0.0f, 0.0f),
	Scale(1.0f, 1.0f, 1.0f),
	RotationQuat(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Keyframe::~Keyframe()
{
}

float BoneAnimation::GetStartTime()const
{
	// Keyframes are sorted by time, so first keyframe gives start time.
	return Keyframes.front().TimePos;
}

float BoneAnimation::GetEndTime()const
{
	// Keyframes are sorted by time, so last keyframe gives end time.
	float f = Keyframes.back().TimePos;

	return f;
}

void BoneAnimation::Interpolate(float t, XMFLOAT4X4& M)const
{
	if (t <= Keyframes.front().TimePos)
	{
		XMVECTOR S = XMLoadFloat3(&Keyframes.front().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.front().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.front().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else if (t >= Keyframes.back().TimePos)
	{
		XMVECTOR S = XMLoadFloat3(&Keyframes.back().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.back().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.back().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else
	{
		for (UINT i = 0; i < Keyframes.size() - 1; ++i)
		{
			if (t >= Keyframes[i].TimePos && t <= Keyframes[i + 1].TimePos)
			{
				float lerpPercent = (t - Keyframes[i].TimePos) / (Keyframes[i + 1].TimePos - Keyframes[i].TimePos);

				XMVECTOR s0 = XMLoadFloat3(&Keyframes[i].Scale);
				XMVECTOR s1 = XMLoadFloat3(&Keyframes[i + 1].Scale);

				XMVECTOR p0 = XMLoadFloat3(&Keyframes[i].Translation);
				XMVECTOR p1 = XMLoadFloat3(&Keyframes[i + 1].Translation);

				XMVECTOR q0 = XMLoadFloat4(&Keyframes[i].RotationQuat);
				XMVECTOR q1 = XMLoadFloat4(&Keyframes[i + 1].RotationQuat);

				XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
				XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
				XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

				break;
			}
		}
	}
}

float AnimationClip::GetClipStartTime()const
{
	// Find smallest start time over all bones in this clip.
	float t = MathHelper::Infinity;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		t = MathHelper::Min(t, BoneAnimations[i].GetStartTime());
	}

	return t;
}

float AnimationClip::GetClipEndTime()const
{
	// Find largest end time over all bones in this clip.
	float t = 0.0f;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		t = MathHelper::Max(t, BoneAnimations[i].GetEndTime());
	}

	return t;
}

void AnimationClip::Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms)const
{
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		// 딱 그 시간에 맞는 프레임을 알기가 어렵다
		// 보간으로 인하여 그래서 어쩔수 없이 반복문을 돌아야함
		BoneAnimations[i].Interpolate(t, boneTransforms[i]);
	}
}

float SkinnedData::GetClipStartTime(const std::string& clipName)const
{
	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipStartTime();
}

float SkinnedData::GetClipEndTime(const std::string& clipName)const
{
	if (!mAnimations.count(clipName)) return 0.f;

	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipEndTime();
}

UINT SkinnedData::BoneCount()const
{
	return mBoneHierarchy.size();
}

void SkinnedData::GetFinalTransforms(const std::string& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms)
{
	if (!mAnimations.count(clipName)) return;
	if (clipName == "") return;

	UINT numBones = mBoneOffsets.size();
	finalTransforms.resize(numBones);

	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	if (clipName == "Jump" || clipName == "Attack")
		timePos *= 1.4f;

	// 이 클립의 모든 뼈대를 주어진 시간에 맞게 보간한다.
	auto clip = mAnimations.find(clipName);
	clip->second.Interpolate(timePos, toParentTransforms);

	//
	// 골격 계통구조를 흝으면서 모든 뼈대를 루트공간으로 변환한다.
	//
	// 루트 트랜스폼 초기화
	m_ToRootTransforms.resize(numBones);

	// 뿌리 뼈대의 인덱스는 0이다. 뿌리 뼈대에는 부모가 없으므로,
	// 뿌리 뼈대의 뿌리 변환은 그냥 자신의 로컬 뼈대 변환이다.
	m_ToRootTransforms[0] = toParentTransforms[0];

	// 이제 자식 뼈대들의 루트 변환들을 구한다.
	// 뼈대들을 흝으면서 부모 뼈대의 루트변환을 다음과 같이 조회한다.
	// ! 항상 부모 뼈대가 자식 뼈대보다 앞에 오게 해야한다.
	for (UINT i = 1; i < numBones; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		// i번 뼈대의 부모의 번호
		int parentIndex = mBoneHierarchy[i];
		// i번 뼈대의 부모의 번호의 변환행렬
		XMMATRIX parentToRoot = XMLoadFloat4x4(&m_ToRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&m_ToRootTransforms[i], toRoot);
	}


	// 뼈대 오프셋 변환을 먼저 곱해서 최종 변환을 구한다.
	for (UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toParentTransforms[i]);
		XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);

		XMStoreFloat4x4(&finalTransforms[i], XMMatrixTranspose(finalTransform));
	}
}

void SkinnedData::GetBlendedAnimationData(const std::string& clipName1, float timePos1, const std::string& clipName2, float timePos2, float factor,
	std::vector<DirectX::XMFLOAT4X4>& finalTransform)
{
	if ((clipName1 == "") || (clipName2 == "")) return;

	std::vector< DirectX::XMFLOAT4X4> bones1;
	std::vector< DirectX::XMFLOAT4X4> bones2;

	// 현재시간 / timepos2 = 0.0f
	GetFinalTransforms(clipName1, timePos1, bones1);
	GetFinalTransforms(clipName2, timePos2, bones2);

	bones1.resize(finalTransform.size());
	bones2.resize(finalTransform.size());

	if (factor < 0.0f) factor = 0.0f;
	if (factor > 1.0f) factor = 1.0f;

	XMFLOAT4X4 temp;

	for (unsigned int boneIndex = 0; boneIndex < finalTransform.size(); ++boneIndex)
	{
		temp._11 = (bones1[boneIndex]._11 * (1.0f - factor)) + (bones2[boneIndex]._11 * factor);
		temp._12 = (bones1[boneIndex]._12 * (1.0f - factor)) + (bones2[boneIndex]._12 * factor);
		temp._13 = (bones1[boneIndex]._13 * (1.0f - factor)) + (bones2[boneIndex]._13 * factor);
		temp._14 = (bones1[boneIndex]._14 * (1.0f - factor)) + (bones2[boneIndex]._14 * factor);

		temp._21 = (bones1[boneIndex]._21 * (1.0f - factor)) + (bones2[boneIndex]._21 * factor);
		temp._22 = (bones1[boneIndex]._22 * (1.0f - factor)) + (bones2[boneIndex]._22 * factor);
		temp._23 = (bones1[boneIndex]._23 * (1.0f - factor)) + (bones2[boneIndex]._23 * factor);
		temp._24 = (bones1[boneIndex]._24 * (1.0f - factor)) + (bones2[boneIndex]._24 * factor);

		temp._31 = (bones1[boneIndex]._31 * (1.0f - factor)) + (bones2[boneIndex]._31 * factor);
		temp._32 = (bones1[boneIndex]._32 * (1.0f - factor)) + (bones2[boneIndex]._32 * factor);
		temp._33 = (bones1[boneIndex]._33 * (1.0f - factor)) + (bones2[boneIndex]._33 * factor);
		temp._34 = (bones1[boneIndex]._34 * (1.0f - factor)) + (bones2[boneIndex]._34 * factor);

		temp._41 = (bones1[boneIndex]._41 * (1.0f - factor)) + (bones2[boneIndex]._41 * factor);
		temp._42 = (bones1[boneIndex]._42 * (1.0f - factor)) + (bones2[boneIndex]._42 * factor);
		temp._43 = (bones1[boneIndex]._43 * (1.0f - factor)) + (bones2[boneIndex]._43 * factor);
		temp._44 = (bones1[boneIndex]._44 * (1.0f - factor)) + (bones2[boneIndex]._44 * factor);

		finalTransform[boneIndex] = temp;
	}
}


std::vector<int> SkinnedData::GetSubmeshOffset() const
{
	return mSubmeshOffset;
}

void SkinnedData::Set(std::vector<int>& boneHierarchy, std::vector<DirectX::XMFLOAT4X4>& boneOffsets, std::unordered_map<std::string, AnimationClip>* animations)
{
	mBoneHierarchy = boneHierarchy;
	mBoneOffsets = boneOffsets;
	if (animations != nullptr)
	{
		mAnimations = (*animations);
	}
}

void SkinnedData::SetBoneName(std::string boneName)
{
	mBoneName.push_back(boneName);
}

std::vector<std::string> SkinnedData::GetBoneName() const
{
	return mBoneName;
}

void SkinnedData::SetAnimation(AnimationClip inAnimation, std::string inClipName)
{
	mAnimations[inClipName] = inAnimation;
}

void SkinnedData::SetSubmeshOffset(int num)
{
	mSubmeshOffset.push_back(num);
}
