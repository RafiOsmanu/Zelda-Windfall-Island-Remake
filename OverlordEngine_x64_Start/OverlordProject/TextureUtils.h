#pragma once
#include "stdafx.h"
#include <codecvt>
#include <string>

namespace textureUtils 
{
	static std::wstring narrowToWide(const std::string& narrowString)
	{
		int length = MultiByteToWideChar(CP_UTF8, 0, narrowString.c_str(), -1, nullptr, 0);
		wchar_t* buffer = new wchar_t[length];
		MultiByteToWideChar(CP_UTF8, 0, narrowString.c_str(), -1, buffer, length);
		std::wstring wideString(buffer);
		delete[] buffer;
		return wideString;
	}

	template<typename MaterialType>
	static void AssignMaterials(ModelComponent* mesh, std::ifstream& objFile, std::ifstream& mtlFile, std::wstring& folderName)
	{
		// Check if the files opened correctly
		if (!mtlFile.is_open()) return;
		if (!objFile.is_open()) return;

		// Initialize variables
		std::string line;
		std::string currentMeshName;
		std::unordered_map<std::string, MaterialType*> materials;
		std::unordered_map<std::string, std::string> materialsNamesDebug;
		std::vector<MaterialType*> materialsVector;

		

		while (std::getline(objFile, line)) {
			// Check if the line defines a new mesh
			if (line.find("usemtl ") != std::string::npos) {
				// Extract the mesh name from the line
				currentMeshName = line.substr(7);

				// Check if the material for the mesh has already been created
				if (materials.find(currentMeshName) == materials.end()) {
					// Find the corresponding material name in the MTL file
					std::string materialName = "default";
					std::string mtlLine;
					bool found = false;

					while (std::getline(mtlFile, mtlLine)) {

						if (mtlLine.find(currentMeshName) != std::string::npos) {
							// Extract the material name from the line
							while (std::getline(mtlFile, mtlLine))
							{
								if (mtlLine.find("map_Kd") != std::string::npos) {
									// Extract the material name from the line
									materialName = mtlLine.substr(7);
									auto pMaterial = MaterialManager::Get()->CreateMaterial<MaterialType>();

									pMaterial->SetDiffuseTexture(L"Textures/" + folderName + L"/" + narrowToWide(materialName));
									materials[currentMeshName] = pMaterial;
									materialsNamesDebug[currentMeshName] = materialName;
									materialsVector.push_back(pMaterial);
									found = true;
									break;
								}
							}
						}
						if (found)
							break;
					}
					mtlFile.clear();
					mtlFile.seekg(0);
				}
			}
		}

		UINT8 size = static_cast<UINT8>(materialsNamesDebug.size());
		for (UINT8 pos{ 0 }; pos < size; ++pos)
		{
			mesh->SetMaterial(materialsVector.at(pos), pos);
		}

		// Close the files
		objFile.close();
		mtlFile.close();
	}


}

