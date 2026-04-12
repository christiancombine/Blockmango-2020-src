#ifndef __LORD_FILERESOURCEMANAGER_H__
#define __LORD_FILERESOURCEMANAGER_H__
#include "TypeDef.h"
#include "Object/Singleton.h"
#include "Object/Root.h"
#include "Util/PathUtil.h"
#include "FileMD5Storage.h"

#ifdef _DEBUG
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define COUNT_OPEN_RESOURCE(name) FileResourceManager::Instance()->countOpenResource(name);
#else
#define COUNT_OPEN_RESOURCE(name)
#endif
#else
#define COUNT_OPEN_RESOURCE(name)
#endif

namespace LORD
{
	class LORD_CORE_API FileResourceManager : public Singleton<FileResourceManager>
	{
	public:
		static FileResourceManager* Instance();
		static String GetResourceCacheFolderName() {
			return RESOURCE_CACHE_FOLDER_NAME;
		};

		//IO读取，接收全路径
		static bool ReadFileAsText(const String& path, String& ret);
		//IO写入，接收全路径
		static bool WriteTextToFile(const String& path, const char* data, const size_t size);
		//String封装为DataStream
		static std::unique_ptr<DataStream> stringToDataStream(const String & resName, const String& content);
	public:
		static String RESOURCE_CACHE_FOLDER_NAME;
	public:
		FileResourceManager(const String& gameType);
		virtual ~FileResourceManager();
		void InitResourcesConfig();

		//模糊加载资源的接口，允许传入文件名或者相对路径
		std::unique_ptr<DataStream> OpenResourceByResName(const String& resName, bool useCompressedTex = false);
		std::unique_ptr<DataStream> OpenResourceByResName(const String& resName, String& retImgExt, bool useCompressedTex = false);
		std::unique_ptr<DataStream> _OpenResourceByResName(const String& resName);
		//返回资源在本地磁盘的全路径，为了兼容老的归档
		String GetResourceLocalFullPath(const String& resourceName);

		//从Game包里加载资源，包括本地Game和Game的Zip,返回DataStream智能指针，接收相对路径
		std::unique_ptr<DataStream> OpenResourceInGame(const String& resRelativePath);
		//从Media包里加载资源，包括本地Media和Media的Zip,返回DataStream智能指针，接收相对路径
		std::unique_ptr<DataStream> OpenResourceInMedia(const String& resRelativePath);
		//从Media包里加载资源，包括本地Media和Media的Zip,返回stringstream智能指针，接收相对路径
		std::unique_ptr<std::stringstream> OpenResourceAsIsInGame(const String& resRelativePath);
		//从Game包里加载资源，包括本地Game和Game的Zip,返回String，接收相对路径
		String OpenResourceAsStrInGame(const String& resRelativePath);
		//从Media包里加载资源，包括本地Media和Media的Zip,返回String，接收相对路径
		String OpenResourceAsStrInMedia(const String& resRelativePath);
		//Zip的IO写入，接收相对路径
		bool WriteTextToGameZipFile(const String& resRelativePath,  const String & data);
		//Zip的IO修改文件时间，接收相对路径
		bool TouchToGameZip(const String& resRelativePath);
		//提供给Lua的IO读取接口，接收全路径
		String LuaReadFileAsText(const String& path);
		//资源是否存在磁盘，接收全路径和相对路径
		bool ResourceExistInDisk(const String& path);
		//资源是否存在Media或游戏目录内，接收相对路径，相对路径会拼接上根路径和游戏包路径组成全路径并返回
		const String ResourceExistInMediaOrGame(const String& resRelativePath);
		//资源是否存在zip内，接收zip的相对路径
		bool ResourceExistInZip(const String& resRelativePath);
		//读取zip文件，接收zip的相对路径、相对路径
		String ReadResourceInZip(const String & path);
		//艺术资源是否已在下载目录内
		bool ResourceExistInDownload(const String& resourceName);
		//资源是否存在任何地方，包括：本地磁盘，本地的下载目录Cache,Zip
		bool ResourceExistInAny(const String& resourceName, bool includeOldPathFind = true);

		//全路径返回相对路径
		const char * FullPathToRelativePath(const char * fullPath, bool ignoreMedia = false);
		// 资源名字返回Md5标识符的全路径
		const String NameToMd5Path(const String & resName);
		//判断资源是否有Md5标识符
		inline bool CheckMD5(String& resName) 
		{ 
			return CheckMD5(resName, true) || CheckMD5(resName, false); 
		}
		bool CheckMD5(String & resName, bool isGame);
		//资源名字返回Md5标识符
		inline const String NameToMd5(const String& resRelativePath)
		{
			return PathUtil::GetPureFilename(NameToDownloadMd5(resRelativePath));
		}
		const String& NameToDownloadMd5(const String& resRelativePath)
		{
			auto it = mNameToMd5GameMap.find(resRelativePath);
			if (it != mNameToMd5GameMap.end())
			{
				return it->second;
			}
			it = mNameToMd5MediaMap.find(resRelativePath);
			if (it != mNameToMd5MediaMap.end())
			{
				return it->second;
			}
			return resRelativePath;
		}
		inline const String& GetResCachePath() {
			return mResCachePath;
		}

		StringArray GetGameCustomFileNameIndexFolderList() {
			return mGameCustomFileNameIndexFolderList;
		};

		void AddGameCustomFileNameIndexFolderList(const String& folderName) {
			mGameCustomFileNameIndexFolderList.push_back(folderName);
		};

		void DumpOpenResourceCount();
		void ResCacheToOriginRes();
		void countOpenResource(const String& resourceName);

		void setGameTyep(const String& gameType) { mGameType = gameType; }

		inline String getRelaPathByName(String& name)
		{
			auto it = mNameToRelaPathMap.find(name);
			if (it == mNameToRelaPathMap.end())
				return name;
			else
				return it->second;
		}

		void setSearchPureName(bool state) { searchPureName = state; }

		const FileMD5Storage& getFileMD5Storage() const { return fileMD5Storage; }
	private:
		void loadResourcesMD5(String& sData, bool isGame);
		//资源相对路径返回游戏包全路径
		inline String joinToGameFullPath(const String& resRelativePath);
		//资源相对路径返回Media全路径
		inline String joinToMediaPath(const String& resRelativePath);

		//内部接口,资源相对路径读取在游戏包内的数据，包括本地和Zip,返回String
		inline bool readAsBinaryInGame(const String& resRelativePath, String& ret);
		//内部接口,资源相对路径读取在Media包内的数据，包括本地和Zip,返回String
		inline bool readAsBinaryInMedia(const String& resRelativePath, String& ret);

		//内部接口,通过md5加载本地缓存资源,返回DataStream智能指针
		std::unique_ptr<DataStream> openResourceByMd5(const String& resRelativePath);
		//内部接口,通过md5加载本地缓存资源,返回String
		String openResourceAsStrByMd5(const String& resRelativePath);
		//内部接口,通过md5加载本地缓存资源,返回stringstream智能指针
		std::unique_ptr<std::stringstream> openResourceAsIsByMd5(const String& resRelativePath);
		void addNameToMd5Pair(const String& resName, const String& md5);
	private:
		FileMD5Storage fileMD5Storage;
		map<String, String>::type mNameToMd5GameMap;
		map<String, String>::type mNameToMd5MediaMap;
		map<String, String>::type mNameToRelaPathMap;
		String mGameType;
		String mResCachePath;
		//游戏包的自定义可以通过文件名查找到文件全路径的几个文件夹，这是以前的资源管理系统遗留的问题
		StringArray mGameCustomFileNameIndexFolderList;

		map<String, int>::type openResourceCounts;
		Mutex mNameToMd5FullPathMapLock;
		bool searchPureName = true;
	};
}
#endif // !__LORD_FILERESOURCEMANAGER_H__
