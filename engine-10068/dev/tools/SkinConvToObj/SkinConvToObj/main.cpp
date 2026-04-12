#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <unordered_set>
#include "windows.h"
#include "..\..\..\..\engine-core\dev\engine\Dependencies\rapidjson\Include\rapidjson\rapidjson.h"
#include "..\..\..\..\engine-core\dev\engine\Dependencies\rapidjson\Include\rapidjson\writer.h"
#include "..\..\..\..\engine-core\dev\engine\Dependencies\rapidjson\Include\rapidjson\document.h"
#include "..\..\..\..\engine-core\dev\engine\Dependencies\rapidjson\Include\rapidjson\reader.h"
#include "..\..\..\..\engine-core\dev\engine\Dependencies\rapidjson\Include\rapidjson\stringbuffer.h"
#include "..\..\..\..\engine-core\dev\engine\Dependencies\RapidXml\Include\rapidxml.hpp"
#include "..\..\..\..\engine-core\dev\engine\Dependencies\RapidXml\Include\rapidxml_utils.hpp"
#include "..\..\..\..\engine-core\dev\engine\Dependencies\RapidXml\Include\rapidxml_print.hpp"
#include "..\..\..\..\engine-core\dev\engine\Dependencies\rapidjson\Include\rapidjson\prettywriter.h"
#include <io.h>
#include <direct.h>

FILE* file;
typedef unsigned int     uint;
typedef unsigned char    Byte;
typedef unsigned short	 Word;
rapidjson::Document address1;
FILE* m_pMeshStream;


void getAllFiles(std::string path, std::map<std::string, std::string>& fileMap)
{
	long   hFile = 0; //文件句柄 
	struct _finddata_t fileinfo; //文件信息  
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))  //判断是否为文件夹
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					fileMap[fileinfo.name] = p.assign(path).append("/").append(fileinfo.name); //保存文件夹名字
					getAllFiles(p.assign(path).append("/").append(fileinfo.name), fileMap);//递归当前文件夹
				}
			}
			else    //文件处理
			{
				std::string filename(fileinfo.name);
				std::string suffix = filename.substr(filename.find_last_of("."), filename.length());
				if (suffix == ".skin" || suffix == ".mesh")
					fileMap[fileinfo.name] = p.assign(path).append("/").append(fileinfo.name); //文件名
			}
		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
		_findclose(hFile);
	}
}

void print(const std::map<std::string, std::unordered_set<std::string>>& map, const std::string& filename)
{
	//init document
	rapidjson::Document document;
	auto& obj = document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator(); //获取分配器  

	for (const auto& item : map)
	{
		rapidjson::Value k, v(rapidjson::kStringType), array(rapidjson::kArrayType);
		if (!item.second.empty())
		{
			k.SetString(item.first.c_str(), item.first.size(), allocator);
			for (const auto& it : item.second)
			{
				v.SetString(it.c_str(), it.size(), allocator);
				array.PushBack(v, allocator);
			}
			obj.AddMember(k.Move(), array.Move(), allocator);
		}
	}

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> pretty_writer(buffer);
	document.Accept(pretty_writer);
	std::ofstream fout; //输出到json文件  
	fout.open(filename); //输出.json文件名
	fout << buffer.GetString();
	fout.close();
}

std::map<std::string, std::unordered_set<std::string>> CreateJsonFile(const std::string& actorPath, std::map<std::string, std::string>& fileMap) try
{
	std::map<std::string, std::unordered_set<std::string>> map;
	std::map<std::string, std::unordered_set<std::string>> conversionMap;

	rapidxml::file<> fdoc(actorPath.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());
	std::cout << doc.name() << std::endl;

	rapidxml::xml_node<>* pRootNode = doc.first_node();
	if (!pRootNode)
	{
		std::cout << "The ActorTemplate file content is valid." << std::endl;
		throw 0;
	}

	rapidxml::xml_node<>* pNode = pRootNode->first_node();
	pNode = pNode->first_node();
	while (pNode)
	{
		if (std::string(pNode->name()) != "BodyParts")
		{
			pNode = pNode->next_sibling();
			continue;
		}

		rapidxml::xml_node<>* pnSubNode = static_cast<rapidxml::xml_node<>*>(pNode);
		rapidxml::xml_node<>* nNode = pnSubNode->first_node();

		std::string key;                              //MasterName.SlaveName:
		while (nNode)
		{
			if (std::string(nNode->name()) != "BodyPart")
			{
				nNode = nNode->next_sibling();
				continue;
			}

			rapidxml::xml_attribute<>* pVarNode = nNode->first_attribute();
			key.clear();
			while (pVarNode)
			{
				std::string varlueName = pVarNode->name();
				if (varlueName == "MasterName")
				{
					key.append(pVarNode->value()).append(".");
				}
				else if (varlueName == "SlaveName")
				{
					key.append(pVarNode->value());
				}
				pVarNode = pVarNode->next_attribute();
			}

			rapidxml::xml_node<>* pChildNode = nNode->first_node();
			while (pChildNode)
			{
				std::string childName = pChildNode->name();
				if (childName == "Skin")
				{
					rapidxml::xml_attribute<>* pSkinElement = pChildNode->first_attribute();
					std::string strSkinName = pSkinElement->name();
					if (strSkinName == "MeshName")
					{
						std::string skinValue = pSkinElement->value();
						std::string skinPath = fileMap[skinValue];
						if ((m_pMeshStream = fopen(skinPath.c_str(), "rb+")) == nullptr)
						{
							std::cout << "文件打开失败！" << skinValue << std::endl;
							break;
						}

						//提取对应的skin文件里的 xx.tga字符串 并生成 MeshName-masterName.slaveName表
						std::string keyAddStr = key;
						if (skinValue.find("_l") != std::string::npos)
							keyAddStr = key + ".l";
						else if (skinValue.find("_r") != std::string::npos)
							keyAddStr = key + ".r";
						conversionMap[skinValue].insert(keyAddStr);

						//版本号
						int version;
						fread(&version, 1, sizeof(int), m_pMeshStream);

						// 子模型数量
						int subMeshNum = 0;
						fseek(m_pMeshStream, sizeof(int), 0);
						fread(&subMeshNum, 1, sizeof(int), m_pMeshStream);

						// 预留空间
						fseek(m_pMeshStream, 72, 1);

						for (int i = 0; i < subMeshNum; i++)
						{
							// name
							uint smNameLen;
							char smName[260];
							fread(&smNameLen, 1, sizeof(uint), m_pMeshStream);
							fread(&smName, 1, smNameLen, m_pMeshStream);

							smName[smNameLen] = 0;

							//material
							uint mtrlLen;
							char mtrlPath[30];
							fread(&mtrlLen, 1, sizeof(uint), m_pMeshStream);
							fread(&mtrlPath, 1, mtrlLen, m_pMeshStream);
							char* ptga = strstr(mtrlPath, ".tga");

							bool onloc = false;
							char* value = nullptr;
							if (ptga)
							{
								onloc = true;
								int parLen = ptga - mtrlPath;
								value = (char*)malloc((parLen + 1) * sizeof(char));
								strncpy(value, mtrlPath, parLen);
								value[parLen] = 0;
							}

							if (onloc)
							{
								// 转换后 mesh 转换的鞋子方向不对，增加标识区分 mesh 和 skin 的转出，让 pc 自己处理
								map["$" + keyAddStr].insert(value);
								fclose(m_pMeshStream);
							}
						}
					}
				}

				if (childName == "StaticMesh")
				{
					rapidxml::xml_node<>* nNode = pChildNode->first_node();
					std::string strName = nNode->name();
					if (strName == "FileName")
					{
						std::string strValue = nNode->value();
						rapidxml::xml_attribute<>* pFileElement = nNode->first_attribute();
						std::string fileKey = pFileElement->name();
						if (fileKey == "value")
						{
							std::string fileValue = pFileElement->value();
							std::string meshPath = fileMap[fileValue];
							if ((m_pMeshStream = fopen(meshPath.c_str(), "rb+")) == nullptr)
							{
								std::cout << "文件打开失败！" << fileValue << std::endl;
								break;
							}

							// 子模型数量
							int subMeshNum = 0;
							fread(&subMeshNum, 1, sizeof(uint), m_pMeshStream);

							// 预留空间
							fseek(m_pMeshStream, 76, 1);

							for (int i = 0; i < subMeshNum; i++)
							{
								// name
								uint smNameLen;
								char smName[50];

								fread(&smNameLen, 1, sizeof(uint), m_pMeshStream);
								fread(&smName, 1, smNameLen, m_pMeshStream);
								smName[smNameLen] = 0;

								//material
								uint mtrlLen;
								char mtrlPath[100];
								fread(&mtrlLen, 1, sizeof(uint), m_pMeshStream);
								fread(&mtrlPath, 1, mtrlLen, m_pMeshStream);
								mtrlPath[mtrlLen] = 0;

								bool onloc = false;
								char* ptga = strstr(mtrlPath, ".tga");

								char* value = nullptr;
								if (ptga)
								{
									onloc = true;
									int parLen = ptga - mtrlPath;
									value = (char*)malloc((parLen + 1) * sizeof(char));
									strncpy(value, mtrlPath, parLen);
									value[parLen] = 0;
								}

								if (onloc)
								{
									std::string keyAddStr = key;
									if (fileValue.find("_l") != std::string::npos)
										keyAddStr = key + ".l";
									else if (fileValue.find("_r") != std::string::npos)
										keyAddStr = key + ".r";

									// 转换后 mesh 转换的鞋子方向不对，增加标识区分 mesh 和 skin 的转出，让 pc 自己处理
									map["@" + keyAddStr].insert(value);
									conversionMap[fileValue].insert(keyAddStr);
									fclose(m_pMeshStream);
								}
							}
						}
					}
				}
				pChildNode = pChildNode->next_sibling();
			};
			nNode = nNode->next_sibling();
		}
		pNode = pNode->next_sibling();
	}

	auto offset = actorPath.find_last_of("\\");
	auto subStr = actorPath.substr(offset + 1, actorPath.length());
	offset = subStr.find_last_of(".");
	subStr = subStr.substr(0, offset);

	print(map, subStr + ".json");
	print(conversionMap, subStr + "SkinConvObj.json");

	return conversionMap;
}
catch (...)
{
	throw "未知错误!!!";
}


void createDirectory(const std::string filename)
{
	auto offset = filename.find_last_of("\\");
	auto path = filename.substr(0, offset);
	if (0 != _access(path.c_str(), 0))
	{
		auto command = "mkdir " + path;
		system(command.c_str()); // 简单点，用命令行
	}
}

bool convFile(bool isSkinned, const std::string objPath, const std::string skinPath)
{
	createDirectory(objPath);
	FILE* objfile;
	if ((objfile = fopen(objPath.c_str(), "wt+")) == nullptr)
	{
		std::cout << "转换时，文件打开 .obj 失败！" << strerror(errno) << std::endl;
		system("pause");
	}

	if ((file = fopen(skinPath.c_str(), "rb+")) == nullptr)
	{
		std::cout << "转换时，读取输入文件失败" << std::endl;
		system("pause");
		return false;
	}

	//读取版本号
	uint version;
	fread(&version, 1, sizeof(uint), file);                           // 4
	std::cout << "版本号：" << version << std::endl;
	if (version == 1)
		return true;

	// 子模型数量
	uint subMeshNumber = 0;
	fseek(file, sizeof(uint), 0);
	fread(&subMeshNumber, 1, sizeof(uint), file);                    // 8
	std::cout << "子模型数量：" << subMeshNumber << std::endl;

	// 预留空间
	fseek(file, 72, 1);												// 80

	for (uint i = 0; i < subMeshNumber; i++)
	{
		//name
		uint smNameLen;
		fread(&smNameLen, 1, sizeof(uint), file);                   // 84

		char smName[260];
		fread(&smName, 1, smNameLen, file);
		std::cout << "smName：";
		uint count = 0;
		while (count < smNameLen)
		{
			std::cout << smName[count++];
		}
		std::cout << std::endl;

		//material
		uint mtrlLen;
		char mtrlPath[260];
		fread(&mtrlLen, 1, sizeof(uint), file);
		std::cout << "mtrlLen：" << mtrlLen << std::endl;
		fread(&mtrlPath, 1, mtrlLen, file);
		std::cout << "mtrlPath：";
		count = 0;

		while (count < mtrlLen)
		{
			std::cout << mtrlPath[count++];
		}
		std::cout << std::endl;

		bool isDiffuseVector = mtrlPath[0] != 0;

		//skip 80 bytes 
		bool m_isUseNormal;
		bool m_isUseVertexColor;
		bool m_isUseLightmapUV;
		bool m_isUseTangentBinormal;
		uint lightmapResolution;

		fread(&m_isUseNormal, 1, sizeof(bool), file);
		fread(&m_isUseVertexColor, 1, sizeof(bool), file);
		fread(&m_isUseLightmapUV, 1, sizeof(bool), file);
		fread(&m_isUseTangentBinormal, 1, sizeof(bool), file);
		fread(&lightmapResolution, 1, sizeof(uint), file);
		fseek(file, 72, 1);

		uint offsetOfData = 3;

		if (m_isUseNormal)
			offsetOfData += 3;

		if (m_isUseVertexColor)
			offsetOfData += 1;

		if (isDiffuseVector)
			offsetOfData += 2;

		if (m_isUseLightmapUV)
			offsetOfData += 2;

		// 动画数据(骨骼权重与索引)
		if (isSkinned)
		{
			offsetOfData += 1; //boneIndex
			offsetOfData += 3; //boneWeight
		}

		if (isSkinned && version >= 3)
		{
			uint numBoneInfluenced = 0;
			fread(&numBoneInfluenced, 1, sizeof(uint), file);
			std::cout << "numBoneInfluenced：" << numBoneInfluenced << std::endl;
			fseek(file, sizeof(uint) * numBoneInfluenced, 1);
		}

		//vertices
		uint vertCount;
		fread(&vertCount, 1, sizeof(uint), file);
		std::cout << "vertCount：" << vertCount << std::endl;

		uint vertBuffSize = vertCount * offsetOfData * sizeof(uint);
		Byte* m_vertices = (Byte*)malloc(sizeof(Byte) * vertBuffSize);
		fread(m_vertices, 1, vertBuffSize, file);

		const char* newline = "\n";
		const char* blank = " ";
		const char* V = "v ";
		count = 1;
		while (count <= vertCount * offsetOfData)
		{
			//顶点数据
			if (count % offsetOfData == 1)
			{
				fwrite(V, strlen(V), 1, objfile);
			}

			if (count % offsetOfData <= 3 && count % offsetOfData >= 1)
			{
				char s[100];
				sprintf(s, "%f", ((float*)m_vertices)[count - 1] * 100);
				if (fwrite(s, strlen(s), 1, objfile) != 1)
				{
					std::cout << "写入 vertices 失败！" << std::endl;
					system("pause");
				}
				fwrite(blank, strlen(blank), 1, objfile);
			}

			if (count % offsetOfData == 3)
			{
				fwrite(newline, strlen(newline), 1, objfile);
			}
			count++;
		};
		fwrite(newline, strlen(newline), 1, objfile);

		//法线数据
		float frist, second, three;
		if (m_isUseNormal)
		{
			const char* VN = "vn ";
			uint count = 1;
			while (count <= vertCount * offsetOfData)
			{
				if (count % offsetOfData == 4)
				{
					fwrite(VN, strlen(VN), 1, objfile);
				}

				if (count % offsetOfData <= 6 && count % offsetOfData >= 4)
				{
					char s[100];
					sprintf(s, "%f", ((float*)m_vertices)[count - 1]);
					if (fwrite(s, strlen(s), 1, objfile) != 1)
					{
						std::cout << "写入 normal 失败！" << std::endl;
						system("pause");
					}
					fwrite(blank, strlen(blank), 1, objfile);
				}

				if (count % offsetOfData == 6)
				{
					fwrite(newline, strlen(newline), 1, objfile);
				}
				count++;
			};
		}
		fwrite(newline, strlen(newline), 1, objfile);

		//UV
		if (isDiffuseVector)
		{
			if (!isSkinned)
			{
				std::cout << "not Skinned" << std::endl;
			}

			uint count = 1;
			const char* VT = "vt ";
			while (count <= vertCount * offsetOfData)
			{
				if (count % offsetOfData == 7)
				{
					fwrite(VT, strlen(VT), 1, objfile);
					char s[100];
					sprintf(s, "%f", ((float*)m_vertices)[count - 1]);
					if (fwrite(s, strlen(s), 1, objfile) != 1)
					{
						std::cout << "写入 UV 失败！" << std::endl;
						system("pause");
					}
					fwrite(blank, strlen(blank), 1, objfile);
				}

				if ((count % offsetOfData == 8 && isSkinned) || (count % offsetOfData == 0 && !isSkinned))
				{
					char s[100];
					sprintf(s, "%f", 1.0f - ((float*)m_vertices)[count - 1]);
					if (fwrite(s, strlen(s), 1, objfile) != 1) {
						std::cout << "写入 UV 失败！" << std::endl;
						system("pause");
					}
					fwrite(newline, strlen(newline), 1, objfile);
				}
				count++;
			};
		}
		fwrite(newline, strlen(newline), 1, objfile);

		//indices
		uint faceCount;
		fread(&faceCount, 1, sizeof(uint), file);
		uint m_idxCount = faceCount * 3;
		uint m_idxStride = sizeof(Word);

		uint idxBuffSize = faceCount * 3 * sizeof(Word);
		Byte* m_indices = (Byte*)malloc(sizeof(Byte) * idxBuffSize);
		fread(m_indices, 1, idxBuffSize, file);

		int num = 1;
		count = 1;
		while (count <= faceCount * 3)
		{
			num++;
			if (count % 3 == 1)
			{
				fwrite("f ", strlen("f "), 1, objfile);
			}

			char s[100];
			sprintf(s, "%d", (((Word*)m_indices)[count - 1] + 1));
			if (fwrite(s, strlen(s), 1, objfile) != 1)
			{
				std::cout << "写入 indices 时失败！" << std::endl;
				system("pause");
			}
			else
			{
				fwrite("/", strlen("/"), 1, objfile);
				fwrite(s, strlen(s), 1, objfile);
				fwrite("/", strlen("/"), 1, objfile);
				fwrite(s, strlen(s), 1, objfile);
			}

			if (count % 3 == 0)
			{
				fwrite(newline, strlen(newline), 1, objfile);
			}
			else {
				fwrite(blank, strlen(blank), 1, objfile);
			}
			count++;
		};
		fwrite(newline, strlen(newline), 1, objfile);

		//skip aabb 
		fseek(file, sizeof(float) * 6, 1);
	}

	fclose(file);
	fclose(objfile);
	return true;
}

void SearchFilesAndConversion(const std::map<std::string, std::string>& fileMap, const std::string actorPath
	, const std::map<std::string, std::unordered_set<std::string>>& map)
{
	auto offset = actorPath.find_last_of("\\");
	auto subStr = actorPath.substr(offset + 1, actorPath.length());
	offset = subStr.find_last_of(".");
	subStr = subStr.substr(0, offset);

	std::map<std::string, uint> keyMap;

	for (const auto& item : map)
	{
		if (fileMap.find(item.first) != fileMap.end())
		{
			for (const auto& it : item.second)
			{
				int offset = item.first.find_last_of(".");
				if (offset == -1)
					continue;
				std::string suffix = item.first.substr(offset, item.first.length());

				std::string filename = std::string(".\\ObjFile\\").append(subStr).append("\\").append(it).append(".obj"); 
				if (keyMap.find(it) != keyMap.end())
				{
					auto& count = keyMap[it];
					++count;
					filename = std::string(".\\ObjFile\\").append(subStr).append("\\").append(it).append(".").append(std::to_string(count)).append(".obj");
				}
				else
				{
					keyMap[it] = 0;
				}

				std::cout << std::endl << "尝试解析：" << fileMap.at(item.first) << std::endl;
				convFile(suffix == ".skin", filename, fileMap.at(item.first));
				std::cout << "解析结果：" << filename << std::endl;
			}
		}
	}
}

void parseAndConversion(const std::string& actorPath, std::map<std::string, std::string>& fileMap)
{
	const auto& result = CreateJsonFile(actorPath, fileMap);
	SearchFilesAndConversion(fileMap, actorPath, result);
}


int main() {
	std::map<std::string, std::string> fileMap;
	std::string DATA_DIR = "..\\..\\..\\..\\res\\client\\Media";
	getAllFiles(DATA_DIR, fileMap);//所有文件与文件夹的路径都输出

	parseAndConversion(DATA_DIR + "\\Actor\\boy.actor", fileMap);
	parseAndConversion(DATA_DIR + "\\Actor\\girl.actor", fileMap);
	parseAndConversion(DATA_DIR + "\\Actor\\decoration\\boy_decoration.actor", fileMap);
	parseAndConversion(DATA_DIR + "\\Actor\\decoration\\girl_decoration.actor", fileMap);

	std::cout << std::endl << "===================解析完成=======================" << std::endl << std::endl;
	system("pause");

	return 0;
}

