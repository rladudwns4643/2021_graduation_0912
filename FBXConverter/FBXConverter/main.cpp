
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <io.h>
#include "FbxLoader.h"
using namespace std;

void LoadFileFbx(std::string filaName);
void LoadFolderFbx(std::string findPath, std::string dir);


void LoadFileAnimationFbx(std::string clipName, ::string filaName);
void LoadFolderAnimationFbx(std::string findPath, std::string dir);

void MergeAnimationsFbx(std::string findPath, std::string dir);

int main(void)
{
	// �ϳ��� �޽��� �ٲٰ� ���� ��
	//LoadFileFbx("..//FBXConverter//PolygonAdventureT//SM_Bld_Village_06");

	//�����ȿ� ��� �޽� fbx�� �ٲٰ� ���� ��
	LoadFolderFbx(".//Mesh//*.fbx", "./Mesh/");

	// ���Ͼȿ� ��� �ִϸ��̼� FBX ���� ��ȯ�� ���� ��
	//LoadFolderAnimationFbx(".//Ani//*.fbx", "./Ani/");

	// Ư�� �ִϸ��̼� ���� �ϳ��� ��ȯ�� ���� ��
	// .FBX�� ������ �̸��� �־��־����
	//LoadFileAnimationFbx("Cowboy", "./Ani/");

	 
	// �� ���� ���� �ִ� ��� �ִϸ��̼� �� ���Ϸ� ��ġ��
	//MergeAnimationsFbx(".//Ani//*.fbx", "./Ani/");
	// animationinfo.txt���� � �ִϸ��̼��� �����ϴ��� ������� ����
	// Merge.anim�� �ִϸ��̼� ���� animationinfo.txt�� ������� ���ĳ��� ����

}

void LoadFileFbx(std::string filaName)
{
	FbxLoader fbx;
	std::vector<Vertex> outVertexVector;
	std::vector<std::uint32_t> outIndexVector;
	std::vector<Material> outMaterial;

	fbx.LoadFBX(outVertexVector, outIndexVector, outMaterial, filaName);

}

void LoadFolderFbx(std::string findPath, std::string dir)
{
	struct _finddata_t fd;
	intptr_t handle;
	vector<string> paths;

	if ((handle = _findfirst(findPath.c_str(), &fd)) == -1L)
	{
		cout << "No file in directory!" << endl;
		return;
	}
	do
	{
		cout << fd.name << endl;
		paths.push_back(dir + fd.name);
	} while (_findnext(handle, &fd) == 0);

	_findclose(handle);

	cout << "Fbx Count is " << paths.size() << endl;
	for (int i = 0; i < paths.size(); ++i)
	{
		for (int j = 0; j < 4; ++j)
			paths[i].pop_back();
		LoadFileFbx(paths[i]);
	}
}

void LoadFileAnimationFbx(std::string clipName, ::string filaName)
{
	FbxLoader fbx;
	std::vector<CharacterVertex> outSkinnedVertices;
	std::vector<std::uint32_t> outIndices;
	std::vector<Material> outMaterial;
	SkinnedData outSkinnedInfo;

	fbx.LoadFBX(outSkinnedVertices, outIndices, outSkinnedInfo, clipName, outMaterial, filaName);
}

void LoadFolderAnimationFbx(std::string findPath, std::string dir)
{
	struct _finddata_t fd;
	intptr_t handle;
	vector<string> paths;

	if ((handle = _findfirst(findPath.c_str(), &fd)) == -1L)
	{
		cout << "No file in directory!" << endl;
		return;
	}
	do
	{
		cout << fd.name << endl;
		paths.push_back(fd.name);
	} while (_findnext(handle, &fd) == 0);

	_findclose(handle);

	cout << "Fbx Count is " << paths.size() << endl;
	for (int i = 0; i < paths.size(); ++i)
	{
		for (int j = 0; j < 4; ++j)
			paths[i].pop_back();
		LoadFileAnimationFbx(paths[i].c_str(), dir);
	}

}

void MergeAnimationsFbx(std::string findPath, std::string dir)
{
	struct _finddata_t fd;
	intptr_t handle;
	vector<string> paths;

	if ((handle = _findfirst(findPath.c_str(), &fd)) == -1L)
	{
		cout << "No file in directory!" << endl;
		return;
	}
	do
	{
		cout << fd.name << endl;
		paths.push_back(fd.name);
	} while (_findnext(handle, &fd) == 0);

	_findclose(handle);

	cout << "Fbx Count is " << paths.size() << endl;
	for (int i = 0; i < paths.size(); ++i)
	{
		for (int j = 0; j < 4; ++j)
			paths[i].pop_back();
		LoadFileAnimationFbx(paths[i].c_str(), dir);
	}

	ofstream aniInfo(dir + "animationinfo.txt", ios_base::out);
	for (int i = 0; i < paths.size(); ++i)
	{
		aniInfo << paths[i] << endl;
	}

	for (int i = 0; i < paths.size(); ++i)
	{
		paths[i] = dir + paths[i] + ".anim";
	}

	cout << "merge start" << endl;
	ofstream fileA(dir + "Merge.anim", ios_base::out);
	string str;
	for (int i = 0; i < paths.size(); ++i)
	{
		cout << i << "��°" << endl;
		ifstream fileC(paths[i], ios_base::in);

		copy(istreambuf_iterator<char>(fileC), istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(fileA));
	}



}