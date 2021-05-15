
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
	// 하나의 메쉬만 바꾸고 싶을 때
	//LoadFileFbx("..//FBXConverter//PolygonAdventureT//SM_Bld_Village_06");

	//폴더안에 모든 메쉬 fbx만 바꾸고 싶을 때
	LoadFolderFbx(".//Mesh//*.fbx", "./Mesh/");

	// 파일안에 모든 애니메이션 FBX 파일 변환을 원할 때
	//LoadFolderAnimationFbx(".//Ani//*.fbx", "./Ani/");

	// 특정 애니메이션 파일 하나만 변환을 원할 때
	// .FBX는 제외한 이름만 넣어주어야함
	//LoadFileAnimationFbx("Cowboy", "./Ani/");

	 
	// 한 폴더 내에 있는 모든 애니메이션 한 파일로 합치기
	//MergeAnimationsFbx(".//Ani//*.fbx", "./Ani/");
	// animationinfo.txt에는 어떤 애니메이션이 존재하는지 순서대로 나열
	// Merge.anim는 애니메이션 정보 animationinfo.txt에 순서대로 합쳐놓은 파일

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
		cout << i << "번째" << endl;
		ifstream fileC(paths[i], ios_base::in);

		copy(istreambuf_iterator<char>(fileC), istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(fileA));
	}



}