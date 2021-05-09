#pragma once

///<summary>
/// A Keyframe defines the bone transformation at an instant in time.
///</summary>
struct Keyframe
{
	Keyframe();
	~Keyframe();

	float TimePos;
	DirectX::XMFLOAT3 Translation;
	DirectX::XMFLOAT3 Scale;
	DirectX::XMFLOAT4 RotationQuat;

	bool operator == (const Keyframe& key)
	{
		if (Translation.x != key.Translation.x || Translation.y != key.Translation.y || Translation.z != key.Translation.z)
			return false;

		if (Scale.x != key.Scale.x || Scale.y != key.Scale.y || Scale.z != key.Scale.z)
			return false;

		if (RotationQuat.x != key.RotationQuat.x || RotationQuat.y != key.RotationQuat.y || RotationQuat.z != key.RotationQuat.z || RotationQuat.w != key.RotationQuat.w)
			return false;

		return true;
	}
};

///<summary>
/// A BoneAnimation is defined by a list of keyframes.  For time
/// values inbetween two keyframes, we interpolate between the
/// two nearest keyframes that bound the time.  
///
/// We assume an animation always has two keyframes.
///</summary>
struct BoneAnimation
{
	float GetStartTime()const;
	float GetEndTime()const;

	void Interpolate(float t, DirectX::XMFLOAT4X4& M)const;

	// 이전 키프레임과 현재 키프레임이 필요함
	std::vector<Keyframe> Keyframes;
};

///<summary>
/// AnimationClip 클래스는 '걷기', '뛰기', '공격', '방어' 같은 개별
/// 애니메이션 클립을 대표한다. 하나의 AnimationClip 객체는 애니메이션
/// 클립을 구성하는 BoneAnimation 인스턴스들(뼈대당 하나씩의)을 담는다.
///</summary>
struct AnimationClip
{
	// 이 클릭의 모든 뼈대 중 가장 이른 시작 시간을 돌려준다.
	float GetClipStartTime()const;

	// 이 클립의 모든 뼈대 중 가장 늦은 종료 시간을 돌려준다.
	float GetClipEndTime()const;

	// 이 클립의 각 BoneAnimation을 흝으면서 애니메이션을 보간한다.
	void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms)const;

	// 이 클립을 구성하는 뼈대별 애니메이션들.
	std::vector<BoneAnimation> BoneAnimations;
};

class SkinnedData
{
public:
	UINT BoneCount()const;

	float GetClipStartTime(const std::string& clipName)const;
	float GetClipEndTime(const std::string& clipName)const;

	// 실제 프로젝트에서는, 만일 같은 clipName과 timePos로
	// 이 메서드를 여러 번 호출할 가능성이 크다면 일종의 캐싱
	// 시스템을 도입하는 것이 바람직할 것이다.
	/* i번 뼈대의 부모의 부모의 오프셋 변환을 얻는 코드*/
	void GetFinalTransforms(const std::string& clipName, float timePos, std::vector<DirectX::XMFLOAT4X4>& finalTransforms);
	void GetBlendedAnimationData(const std::string& clipName1, float timePos1, const std::string& clipName2, float timePos2, float factor, std::vector<DirectX::XMFLOAT4X4>& finalTransform);

public:
	void SetBoneName(std::string boneName);
	std::vector<std::string> GetBoneName() const;
	void SetAnimation(AnimationClip inAnimation, std::string inClipName);
	void SetSubmeshOffset(int num);
	std::vector<int> GetSubmeshOffset() const;

	void Set(
		std::vector<int>& boneHierarchy,
		std::vector<DirectX::XMFLOAT4X4>& boneOffsets,
		std::unordered_map<std::string, AnimationClip>* animations = nullptr);

public:
	std::vector<std::string> mBoneName;
	std::vector<int> mBoneHierarchy;
	std::vector<DirectX::XMFLOAT4X4> mBoneOffsets;
	std::vector<XMFLOAT4X4> m_ToRootTransforms;

	std::vector<std::string> mAnimationName;
	std::unordered_map<std::string, AnimationClip> mAnimations;

	std::vector<int> mSubmeshOffset;
};

