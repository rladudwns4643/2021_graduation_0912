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

	// ���� Ű�����Ӱ� ���� Ű�������� �ʿ���
	std::vector<Keyframe> Keyframes;
};

///<summary>
/// AnimationClip Ŭ������ '�ȱ�', '�ٱ�', '����', '���' ���� ����
/// �ִϸ��̼� Ŭ���� ��ǥ�Ѵ�. �ϳ��� AnimationClip ��ü�� �ִϸ��̼�
/// Ŭ���� �����ϴ� BoneAnimation �ν��Ͻ���(����� �ϳ�����)�� ��´�.
///</summary>
struct AnimationClip
{
	// �� Ŭ���� ��� ���� �� ���� �̸� ���� �ð��� �����ش�.
	float GetClipStartTime()const;

	// �� Ŭ���� ��� ���� �� ���� ���� ���� �ð��� �����ش�.
	float GetClipEndTime()const;

	// �� Ŭ���� �� BoneAnimation�� �y���鼭 �ִϸ��̼��� �����Ѵ�.
	void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms)const;

	// �� Ŭ���� �����ϴ� ���뺰 �ִϸ��̼ǵ�.
	std::vector<BoneAnimation> BoneAnimations;
};

class SkinnedData
{
public:
	UINT BoneCount()const;

	float GetClipStartTime(const std::string& clipName)const;
	float GetClipEndTime(const std::string& clipName)const;

	// ���� ������Ʈ������, ���� ���� clipName�� timePos��
	// �� �޼��带 ���� �� ȣ���� ���ɼ��� ũ�ٸ� ������ ĳ��
	// �ý����� �����ϴ� ���� �ٶ����� ���̴�.
	/* i�� ������ �θ��� �θ��� ������ ��ȯ�� ��� �ڵ�*/
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

