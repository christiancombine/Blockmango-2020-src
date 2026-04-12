#include "Core.h"
#include <sys/stat.h>
#include "FileResourceManager.h"
#include "Util/CSVWriter.h"
#include "Util\ChecksumUtil.h"
#include "Util/PathUtil.h"
#include "Util/Finally.h"
#include <cerrno>
#include <cstring>
#include "Util/StringUtil.h"
#include "ResourceGroupManager.h"
//#include "ZipResourceManager.h"
//#include "ResourceDownloadManager.h"
#include "Log/Log.h"

namespace LORD
{
	String FileResourceManager::RESOURCE_CACHE_FOLDER_NAME = "ResCache";

#ifdef _DEBUG
	map<String, String>::type md5ToNameMap;
#endif

	FileResourceManager* FileResourceManager::Instance()
	{
		return ms_pSingleton;
	}
	//-----------------------------------------------------------------------
	FileResourceManager::FileResourceManager(const String& gameType)
	{
		mGameType = gameType;
		mResCachePath = Root::Instance()->getResCachePath();

		mGameCustomFileNameIndexFolderList = {
			"asset",
			"resource/gui",
			"resource/imageset",
			"resource/effects",
			"recharge",
			"font"
		};

		if (mResCachePath.empty()) return;
		auto cacheDeleteThread = std::thread([=]() {
			const time_t CacheExitTime = 86400 * 15;
			const time_t now = time(0);

			StringArray cacheFiles;
			PathUtil::EnumFilesInDir(cacheFiles, mResCachePath, false, false, true);
			struct stat st;
			for (auto file : cacheFiles)
			{
				stat(file.c_str(), &st);
				if (now > st.st_atime + CacheExitTime)
					remove(file.c_str());
			}

			const time_t terrainCacheExitTime = 60 * 60 * 12;
			cacheFiles.clear();
			PathUtil::EnumSubDirs(cacheFiles, mResCachePath + "/terrain", false);
			for (auto file : cacheFiles)
			{
				stat(file.c_str(), &st);
				if (now > st.st_atime + terrainCacheExitTime)
					PathUtil::DelPath(file.c_str());
			}
		});
		cacheDeleteThread.detach();
	}
	//-----------------------------------------------------------------------
	FileResourceManager::~FileResourceManager()
	{

	}
	//-----------------------------------------------------------------------
	static bool is_absolute_path(const char* path)
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		if (path[0] == '.')
			return true;
		if (isalpha(ui8(path[0])) && path[1] == ':')
			return true;
#endif
		return path[0] == '/' || path[0] == '\\';
	}
	//-----------------------------------------------------------------------
	static String concatenate_path(const String& base, const String& name)
	{
		if (base.empty() || is_absolute_path(name.c_str()))
			return name;
		else
			return StringUtil::Format("%s/%s", base.c_str(), name.c_str());
	}
	//-----------------------------------------------------------------------
	void FileResourceManager::InitResourcesConfig() {
		String ret = OpenResourceAsStrInMedia("Media/Media_res_config.txt");
		loadResourcesMD5(ret, false);
		ret = OpenResourceAsStrInGame(StringUtil::Format("%s_res_config.txt", mGameType.c_str()));
		if (ret.empty())
		{
			ret = OpenResourceAsStrInGame(StringUtil::Format("res_config.txt", mGameType.c_str()));
		}
		loadResourcesMD5(ret, true);

		ret = OpenResourceAsStrInGame(StringUtil::Format("game_res_config.txt"));
		fileMD5Storage.parse(ret);
		std::map<String, String> resToDownloadMD5;
		fileMD5Storage.getKeyToDownloadMD5Map(resToDownloadMD5);
		for (auto pair : resToDownloadMD5)
		{
			addNameToMd5Pair(pair.first, pair.second);
		}

		ret = OpenResourceAsStrInMedia("Media/Decorate_res_config.txt");
		loadResourcesMD5(ret, false);

	}
	//-----------------------------------------------------------------------
	std::unique_ptr<DataStream> FileResourceManager::OpenResourceByResName(const String& resName, bool useCompressedTex)
	{
		String strImgExt;
		return OpenResourceByResName(resName, strImgExt, useCompressedTex);
	}
	//-----------------------------------------------------------------------
	std::unique_ptr<DataStream> FileResourceManager::OpenResourceByResName(const String& resName, String& retImgExt, bool useCompressedTex)
	{
		retImgExt = PathUtil::GetFileExt(resName, true);
		StringUtil::LowerCase(retImgExt);
		if ((retImgExt == ".png" || retImgExt == ".tga"))
		{
			if (useCompressedTex && false)
			{
				const auto& nameKtx = PathUtil::GetPureFilename(resName, false) + ".ktx";
				std::unique_ptr<DataStream> pResult = _OpenResourceByResName(nameKtx);
				if (pResult)
				{
					retImgExt = ".ktx";
					return pResult;
				}
				return pResult;
			}
#if LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			const auto& nameKtx = resName + ".ktx";
			std::unique_ptr<DataStream> pResult = _OpenResourceByResName(nameKtx);
			if (pResult)
			{
				retImgExt = ".ktx";
				return pResult;
			}
#endif
#if LORD_PLATFORM == LORD_PLATFORM_MAC_IOS
			const auto& nameKtx = resName + ".pvr";
			std::unique_ptr<DataStream> pResult = _OpenResourceByResName(nameKtx);
			if (pResult)
			{
				retImgExt = ".pvr";
				return pResult;
			}
#endif
		}
		
		else
			retImgExt = "";

		auto stream = _OpenResourceByResName(resName);
		if (stream) return stream;
		if (!searchPureName || resName.find(PathUtil::SLASH) == std::string::npos)
			return stream;

		String resFromMd5Name = resName;
		if (!stream && PathUtil::GetPureFilename(resName) != resName && !CheckMD5(resFromMd5Name, true))
			stream = _OpenResourceByResName(PathUtil::GetPureFilename(resName));//如果相对路径不行，尝试用名字。

		return stream;
	}
	//-----------------------------------------------------------------------
	std::unique_ptr<DataStream> FileResourceManager::_OpenResourceByResName(const String & resName)
	{
		if (resName.empty()) return 0;
		String ret;

		//Game本地加载
		String &&gameResFullPath = joinToGameFullPath(resName);
		if (ReadFileAsText(gameResFullPath, ret)) {
			//LordLogInfo("FileResourceManager::openResourceByResName::load [%s] in local Game", resName.c_str());
			return std::move(stringToDataStream(gameResFullPath, ret));
		}
		//从本地的其他目录加载
		String &&relativePath = ResourceGroupManager::Instance()->GetFileRelativePath(resName, true);
		if (is_absolute_path(relativePath.c_str()))
		{
			if (ReadFileAsText(relativePath, ret)) {
				//LordLogInfo("FileResourceManager::openResourceByResName::load [%s] in local other", resName.c_str());
				return std::move(stringToDataStream(relativePath, ret));
			}
		}
		//Game本地其他目录加载
		gameResFullPath = joinToGameFullPath(relativePath);
		if (ReadFileAsText(gameResFullPath, ret)) {
			//LordLogInfo("FileResourceManager::openResourceByResName::load [%s] in local Game", resName.c_str());
			return std::move(stringToDataStream(gameResFullPath, ret));
		}
		//GameZip加载
		//String resGameZipRelativePath = ZipResourceManager::Instance()->GetFileRelativePathInGameZip(resName);
		//if (ZipResourceManager::Instance()->readAsBinaryInGameZip(resGameZipRelativePath, ret))
		//{
		//	//LordLogInfo("FileResourceManager::openResourceByResName::load [%s] in zip Game", resName.c_str());
		//	return std::move(stringToDataStream(resGameZipRelativePath, ret));
		//}
		//通过MD5加载
		String resFromMd5Name = resName;
		if (CheckMD5(resFromMd5Name, true)) 
		{
			return std::move(openResourceByMd5(resFromMd5Name));
			//LordLogInfo("FileResourceManager::openResourceByResName::load [%s] in Cache Game", resName.c_str());
		}
		//Media本地加载
		relativePath = ResourceGroupManager::Instance()->GetFileRelativePath(resName, false);
		const String& mediaResFullPath = joinToMediaPath(relativePath);
		if (PathUtil::IsPathExist(mediaResFullPath))
		{
			if (ReadFileAsText(mediaResFullPath, ret)) {
				//LordLogInfo("FileResourceManager::openResourceByResName::load [%s] in local Media", mediaResLocalRelativePath.c_str());
				return std::move(stringToDataStream(mediaResFullPath, ret));
			}
		}
		//else
		//{
		//	LordLogError("FileResourceManager::openResourceByResName::load faild [%s] in local Media", mediaResLocalRelativePath.c_str());
		//}
		//MediaZip加载
		//String resMdiaZipRelativePath = ZipResourceManager::Instance()->GetFileRelativePathInMediaZip(resName);
		//if (ZipResourceManager::Instance()->readAsBinaryInMediaZip(resMdiaZipRelativePath, ret))
		//{
		//	//LordLogInfo("FileResourceManager::openResourceByResName::load [%s] in zip Media", resName.c_str());
		//	return std::move(stringToDataStream(resMdiaZipRelativePath, ret));
		//}
		//通过MD5加载
		resFromMd5Name = resName;
		if (CheckMD5(resFromMd5Name, false)) 
		{
			return std::move(openResourceByMd5(resFromMd5Name));
		}
		//LordLogInfo("FileResourceManager::openResourceByResName::load faild [%s] in Any", resName.c_str());
		return 0;
	}
	//-----------------------------------------------------------------------
	std::unique_ptr<DataStream> FileResourceManager::OpenResourceInGame(const String & resRelativePath)
	{
		//Game中加载
		String ret;
		if (readAsBinaryInGame(resRelativePath, ret)) {
			return std::move(stringToDataStream(resRelativePath, ret));
		}
		//用MD5来加载资源
		String resFromMd5Name = resRelativePath;
		if (CheckMD5(resFromMd5Name, true)) {
			return std::move(openResourceByMd5(resFromMd5Name));
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	std::unique_ptr<DataStream> FileResourceManager::OpenResourceInMedia(const String & resRelativePath)
	{
		//Media中加载
		String ret;
		if (readAsBinaryInMedia(resRelativePath, ret)) {
			return std::move(stringToDataStream(resRelativePath, ret));
		}
		//用MD5来加载资源
		String resFromMd5Name = resRelativePath;
		if (CheckMD5(resFromMd5Name, false)) {
			return std::move(openResourceByMd5(resFromMd5Name));
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	std::unique_ptr<std::stringstream> FileResourceManager::OpenResourceAsIsInGame(const String & resRelativePath)
	{
		//Game中加载
		String ret;
		if (readAsBinaryInGame(resRelativePath, ret)) {
			std::unique_ptr<std::stringstream> origStream = std::make_unique<std::stringstream>();
			origStream->write(ret.data(), ret.length());
			return std::move(origStream);
		}
		//用MD5来加载资源
		return std::move(openResourceAsIsByMd5(resRelativePath));
	}
	//-----------------------------------------------------------------------
	String FileResourceManager::OpenResourceAsStrInGame(const String & resRelativePath)
	{
		String ret;
		if (readAsBinaryInGame(resRelativePath, ret)) {
			return ret;
		}
		return openResourceAsStrByMd5(resRelativePath);
	}
	//-----------------------------------------------------------------------
	String FileResourceManager::OpenResourceAsStrInMedia(const String & resRelativePath)
	{
		String ret;
		if (readAsBinaryInMedia(resRelativePath, ret)) {
			return ret;
		}
		return openResourceAsStrByMd5(resRelativePath);
	}
	//-----------------------------------------------------------------------
	inline String FileResourceManager::joinToGameFullPath(const String & resRelativePath)
	{
		if (Root::Instance())
		{
			return concatenate_path(Root::Instance()->getGamePath(), resRelativePath.c_str());
		}
		return resRelativePath;
	}
	//-----------------------------------------------------------------------
	inline String FileResourceManager::joinToMediaPath(const String & resRelativePath)
	{
		if (Root::Instance())
		{
			return concatenate_path(Root::Instance()->getRootPath(), resRelativePath.c_str());
		}
		return resRelativePath;
	}
	//-----------------------------------------------------------------------
	inline bool FileResourceManager::readAsBinaryInGame(const String & resRelativePath, String& ret)
	{
		//从本地加载
		if (ReadFileAsText(joinToGameFullPath(resRelativePath), ret)) {
			return true;
		}
		//从Zip加载
		String tempRelativePath = StringUtil::Replace(resRelativePath, "\\", "/");
		//地图传入的是全路径，去掉游戏包根目录
		if (is_absolute_path(tempRelativePath.c_str()))
		{
			tempRelativePath = FullPathToRelativePath(tempRelativePath.c_str());
		}
		/*if (ZipResourceManager::Instance()->readAsBinaryInGameZip(tempRelativePath, ret))
		{
			return true;
		}*/
		return false;
	}
	//-----------------------------------------------------------------------
	inline bool FileResourceManager::readAsBinaryInMedia(const String & resRelativePath, String& ret)
	{
		//从本地加载
		const String& mediaResFullPath = joinToMediaPath(resRelativePath);
		if (ReadFileAsText(mediaResFullPath, ret)) {
			return true;
		}
		//从Zip加载
		String tempRelativePath = StringUtil::Replace(resRelativePath, "\\", "/");
		/*if (ZipResourceManager::Instance()->readAsBinaryInMediaZip(tempRelativePath, ret))
		{
			return true;
		}*/
		return false;
	}
	//-----------------------------------------------------------------------
	std::unique_ptr<DataStream> FileResourceManager::openResourceByMd5(const String & resRelativePath)
	{
		//用MD5来加载资源
		String ret;
		String resCachePath = NameToMd5Path(resRelativePath);
		if (ReadFileAsText(resCachePath, ret)) {
			return std::move(stringToDataStream(resCachePath, ret));
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	String FileResourceManager::openResourceAsStrByMd5(const String & resRelativePath)
	{
		//用MD5来加载资源
		String resFromMd5Name = resRelativePath;
		if (!CheckMD5(resFromMd5Name)) {
			return StringUtil::BLANK;
		}
		String ret;
		String resCachePath = NameToMd5Path(resFromMd5Name);
		if (ReadFileAsText(resCachePath, ret)) {
			return ret;
		}
		return StringUtil::BLANK;
	}
	//-----------------------------------------------------------------------
	std::unique_ptr<std::stringstream> FileResourceManager::openResourceAsIsByMd5(const String & resRelativePath)
	{
		std::unique_ptr<std::stringstream> origStream = std::make_unique<std::stringstream>();
		//用MD5来加载资源
		String resFromMd5Name = resRelativePath;
		if (CheckMD5(resFromMd5Name)) {
			String ret;
			String resCachePath = NameToMd5Path(resFromMd5Name);
			if (ReadFileAsText(resCachePath, ret)) {
				origStream->write(ret.data(), ret.length());
			}
		}
		return std::move(origStream);
	}

	void FileResourceManager::addNameToMd5Pair(const String& oriName, const String& md5)
	{
		mNameToMd5GameMap[oriName] = md5;
		if (oriName.find(PathUtil::SLASH) != String::npos)
		{
			String&& pureName = PathUtil::GetPureFilename(oriName);
			for (const String& folder : FileResourceManager::Instance()->GetGameCustomFileNameIndexFolderList())
			{
				if (StringUtil::StartWith(oriName, folder))
				{
					mNameToMd5GameMap.insert(make_pair(oriName.substr(folder.size() + 1, oriName.size()), md5));
					mNameToMd5GameMap.insert(make_pair(pureName, md5));
				}
			}
			mNameToRelaPathMap.insert(make_pair(pureName, oriName));
		}
#ifdef _DEBUG
		md5ToNameMap.insert(make_pair(md5, oriName));
#endif
	}

	//-----------------------------------------------------------------------
	std::unique_ptr<DataStream> FileResourceManager::stringToDataStream(const String & resName, const String & content)
	{
		std::stringstream * origStream = new std::stringstream();
		//std::stringstream* origStream = LORD_NEW_T(std::stringstream);
		origStream->write(content.data(), content.length());
		return std::move(std::make_unique<FileStreamDataStream>(resName, origStream, content.length(), true, false));
	}
	//-----------------------------------------------------------------------
	bool FileResourceManager::ReadFileAsText(const String & path, String& ret)
	{
#define RETURN_ON_FAIL(cond, msg) if (cond) { LordLogError("%s: %s", msg, strerror(errno)); return false; }
		FILE* file = fopen(path.c_str(), "rb");
		if (!file)
			return false;
		auto closeFile = finally([&] { fclose(file); });
		RETURN_ON_FAIL(fseek(file, 0, SEEK_END), "ReadFileAsText fseek fail 1");
		auto size = ftell(file);
		RETURN_ON_FAIL(size < 0, "ReadFileAsText ftell fail");
		RETURN_ON_FAIL(fseek(file, 0, SEEK_SET), "ReadFileAsText fseek fail 2");
		char* data = static_cast<char*>(LordMalloc(size));
		if (!data)
		{
			LordLogError("ReadFileAsText failed to allocate memory: %lld", (long long)size);
			return false;
		}
		auto deleteData = finally([&] { LordFree(data); });
		auto readSize = fread(data, sizeof(char), size, file);
		ret = String(data, readSize);
		return true;
#undef RETURN_ON_FAIL
	}
	//-----------------------------------------------------------------------
	bool FileResourceManager::WriteTextToFile(const String & path, const char * data, const size_t size)
	{
		std::fstream f;
		f.open(path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
		LORD::DataStream* filePtr(LordNew LORD::FileStreamDataStream(&f));
		if (!filePtr)
		{
			return false;
		}
		filePtr->write(data, size);
		f.close();
		return true;
	}
	bool FileResourceManager::WriteTextToGameZipFile(const String & resRelativePath, const String & data)
	{
		return false;
	}
	bool FileResourceManager::TouchToGameZip(const String & resRelativePath)
	{
		return false;
	}
	//-----------------------------------------------------------------------
	String FileResourceManager::LuaReadFileAsText(const String & path)
	{
		String ret;
		if (ReadFileAsText(path, ret)) {
			return ret;
		}
		return ret;
	}
	//-----------------------------------------------------------------------
	bool FileResourceManager::ResourceExistInDisk(const String & path)
	{
		size_t size = 0;
#if LORD_PLATFORM == LORD_PLATFORM_ANDROID
		static const String extsSrc[] = { ".png", ".tga" };
		static const String extsDst[] = { ".ktx", ".ktx" };
		for (int i = 0; i < 2; i++)
		{
			const String& extSrc = extsSrc[i];
			const String& extDst = extsDst[i];
			if (StringUtil::EndWith(path, extSrc))
				if (PathUtil::IsPathExist(PathUtil::GetRenameExtFile(path, extDst), &size) && size > 0)
					return true;
		}
#elif LORD_PLATFORM == LORD_PLATFORM_MAC_IOS
		static const String extsSrc[] = { ".png", ".tga" };
		static const String extsDst[] = { ".pvr", ".pvr" };
		for (int i = 0; i < 2; i++)
		{
			const String& extSrc = extsSrc[i];
			const String& extDst = extsDst[i];
			if (StringUtil::EndWith(path, extSrc))
				if (PathUtil::IsPathExist(PathUtil::GetRenameExtFile(path, extDst), &size) && size > 0)
					return true;
		}
#endif
		return PathUtil::IsPathExist(path, &size) && size > 0;
	}
	//-----------------------------------------------------------------------
	const String FileResourceManager::ResourceExistInMediaOrGame(const String & resRelativePath)
	{
		String fullPath = joinToGameFullPath(resRelativePath);
		if (PathUtil::IsPathExist(fullPath)) {
			return fullPath;
		}
		fullPath = joinToGameFullPath(resRelativePath + ".dat");
		if (PathUtil::IsPathExist(fullPath)) {
			return fullPath;
		}
		if (resRelativePath.find("/") != String::npos) {
			String pureFileName = PathUtil::GetPureFilename(resRelativePath);
			String gameResLocalRelativePath = ResourceGroupManager::Instance()->GetFileRelativePath(pureFileName, true);
			fullPath = joinToGameFullPath(gameResLocalRelativePath);
			if (PathUtil::IsPathExist(fullPath.c_str())) {
				return fullPath;
			}
		}
		fullPath = joinToMediaPath(resRelativePath);
		if (PathUtil::IsPathExist(fullPath.c_str())) {
			return fullPath;
		}
		if (resRelativePath.find("/") != String::npos) {
			String pureFileName = PathUtil::GetPureFilename(resRelativePath);
			String mediaResLocalRelativePath = ResourceGroupManager::Instance()->GetFileRelativePath(pureFileName, false);
			fullPath = joinToMediaPath(mediaResLocalRelativePath);
			if (PathUtil::IsPathExist(fullPath.c_str())) {
				return fullPath;
			}
		}
		return StringUtil::BLANK;
	}
	//-----------------------------------------------------------------------
	bool FileResourceManager::ResourceExistInZip(const String & resRelativePath)
	{
#if LORD_PLATFORM == LORD_PLATFORM_ANDROID
		static const String extsSrc[] = { ".png", ".tga" };
		static const String extsDst[] = { ".ktx", ".ktx" };
		for (int i = 0; i < 2; i++)
		{
			const String& extSrc = extsSrc[i];
			const String& extDst = extsDst[i];
			if (StringUtil::EndWith(resRelativePath, extSrc))
				if (ZipResourceManager::Instance()->resourceExistsInZipArchive(PathUtil::GetRenameExtFile(resRelativePath, extDst)))
					return true;
		}
#elif LORD_PLATFORM == LORD_PLATFORM_MAC_IOS
		static const String extsSrc[] = { ".png", ".tga" };
		static const String extsDst[] = { ".pvr", ".pvr" };
		for (int i = 0; i < 2; i++)
		{
			const String& extSrc = extsSrc[i];
			const String& extDst = extsDst[i];
			if (StringUtil::EndWith(resRelativePath, extSrc))
				if (ZipResourceManager::Instance()->resourceExistsInZipArchive(PathUtil::GetRenameExtFile(resRelativePath, extDst)))
					return true;
		}
#endif
		return false;
	}
	String FileResourceManager::ReadResourceInZip(const String & path)
	{
		return "";
	}
	//-----------------------------------------------------------------------
	bool FileResourceManager::ResourceExistInDownload(const String & resourceName)
	{
		/*if (ResourceDownloadManager::Instance() && ResourceDownloadManager::Instance()->resourceIsDownloading(resourceName))
			return false;*/

		return ResourceExistInDisk(NameToMd5Path(resourceName));
	}
	//-----------------------------------------------------------------------
	bool FileResourceManager::ResourceExistInAny(const String & resourceName, bool includeOldPathFind)
	{
		if (resourceName.size() == 0)
			return false;

		String fullPath = joinToGameFullPath(resourceName);
		if (ResourceExistInDisk(fullPath))
			return true;
		String relativePath;
		//看下是不是旧的资源管理拼接路径的资源
		if (includeOldPathFind)
		{
			relativePath = ResourceGroupManager::Instance()->GetFileRelativePath(resourceName, true);
			fullPath = joinToGameFullPath(relativePath);
		}
		else
			relativePath = resourceName;
		//是否是游戏资源
		if (ResourceExistInDisk(fullPath))
			return true;
		String fullPathWithDat = fullPath + ".dat";
		if (ResourceExistInDisk(fullPathWithDat))
			return true;
		if (ResourceExistInZip(relativePath))
			return true;
		if (CheckMD5(relativePath, true))
			return ResourceExistInDisk(NameToMd5Path(relativePath));
		//是否是Media资源
		relativePath = ResourceGroupManager::Instance()->GetFileRelativePath(resourceName, false);
		fullPath = joinToMediaPath(relativePath);
		if (ResourceExistInDisk(fullPath))
			return true;
		if (CheckMD5(relativePath, false))
			return ResourceExistInDisk(NameToMd5Path(relativePath));
		return false;
	}
	//-----------------------------------------------------------------------
	String FileResourceManager::GetResourceLocalFullPath(const String & resourceName)
	{
		return ResourceGroupManager::Instance()->GetFileLocation(resourceName);
	}
	//-----------------------------------------------------------------------
	const char * FileResourceManager::FullPathToRelativePath(const char * fullPath, bool ignoreMedia)
	{
		size_t length = Root::Instance()->getGamePath().length();
		String gamePath = StringUtil::Replace(Root::Instance()->getGamePath(), "\\", "/");
		if (StringUtil::StartWith(fullPath, gamePath)) 
		{
			return 	 fullPath + length;
		}
		if (ignoreMedia)
			return fullPath;

		length = Root::Instance()->getRootPath().length();
		if (StringUtil::StartWith(fullPath, Root::Instance()->getRootPath())) 
		{
			return 	 fullPath + length;
		}
		return fullPath;
	}
	//-----------------------------------------------------------------------
	const String FileResourceManager::NameToMd5Path(const String & resName)
	{
		String resFromMd5Name = resName;
		if (CheckMD5(resFromMd5Name)) {
			auto&& md5 = NameToMd5(resFromMd5Name);
			return StringUtil::Format("%s/%s", Root::Instance()->getResCachePath().c_str(),  md5.c_str());
		}
		return resName;
	}

	bool FileResourceManager::CheckMD5(String & resName, bool isGame)
	{
		if (isGame)
		{
			if (mNameToMd5GameMap.find(resName) != mNameToMd5GameMap.end())
			{
				return true;
			}
			if (mNameToMd5MediaMap.find(resName) != mNameToMd5MediaMap.end())
			{
				return false;
			}
			if (resName.find("/") != String::npos) {
				String pureFileName = PathUtil::GetPureFilename(resName);
				if (mNameToMd5GameMap.find(pureFileName) != mNameToMd5GameMap.end())
				{
					resName = pureFileName;
					return true;
				}
			}
		}
		else
		{
			if (mNameToMd5MediaMap.find(resName) != mNameToMd5MediaMap.end())
			{
				return true;
			}
			if (mNameToMd5GameMap.find(resName) != mNameToMd5GameMap.end())
			{
				return false;
			}
			if (resName.find("/") != String::npos) {
				String pureFileName = PathUtil::GetPureFilename(resName);
				if (mNameToMd5MediaMap.find(pureFileName) != mNameToMd5MediaMap.end())
				{
					resName = pureFileName;
					return true;
				}
			}
		}
		return false;
	}
	//-----------------------------------------------------------------------
	void FileResourceManager::loadResourcesMD5(String& sData, bool isGame)
	{
		if (sData.empty())
		{
			return;
		}
		String src = "/";
		sData = StringUtil::Replace(sData, "\\", "/");
		sData = StringUtil::Replace(sData, " = ", "=");
		StringArray&& lines = StringUtil::Split(sData, "\r\n");
		for (const auto& line : lines)
		{
			StringArray&&  tempVec = StringUtil::Split(line, "=");
			if (tempVec.size() == 2)
			{
				const String& oriName = tempVec[0];
				const String& md5 = tempVec[1];

				if (isGame)
				{
					addNameToMd5Pair(oriName, md5);
				}
				else
				{
					mNameToMd5MediaMap[oriName] = md5;
					if (oriName.find(src) != String::npos)
					{
						String&& pureName = PathUtil::GetPureFilename(oriName);
						mNameToMd5MediaMap.insert(make_pair(pureName, md5));
						mNameToRelaPathMap.insert(make_pair(pureName, oriName));
					}
#ifdef _DEBUG
					md5ToNameMap.insert(make_pair(md5, oriName));
#endif
				}
			}
			else
			{
				LordLogError("Invalid format in file , %s", line.c_str());
			}
		}
	}

	void FileResourceManager::countOpenResource(const String& resourceName)
	{
		auto it = openResourceCounts.find(resourceName);
		if (it == openResourceCounts.end())
		{
			openResourceCounts.insert(std::make_pair(resourceName, 1));
		}
		else
		{
			it->second++;
		}
	}

	void FileResourceManager::DumpOpenResourceCount()
	{
#ifdef _DEBUG
		const String filePath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "OpenResCount.xls");
		std::vector<std::string> header =
		{
			"ResourcePath", "OpenedCount"
		};
		CsvWriter* pWriter = new CsvWriter(filePath.c_str(), header);
		pWriter->CreateNew();

		for (auto iter : openResourceCounts)
		{
			std::map<String, String> result =
			{
				{ "ResourcePath", iter.first },
				{ "OpenedCount", std::to_string(iter.second).c_str() },
			};
			pWriter->Write(result);
		}

		pWriter->Close();
		LordSafeDelete(pWriter);
#endif
	}
	//---------------------------------------------------------------------
	void FileResourceManager::ResCacheToOriginRes()
	{
#ifdef _DEBUG
		StringArray cacheFiles;
		PathUtil::EnumFilesInDir(cacheFiles, Root::Instance()->getResCachePath());

		const String outPathMediaRes = StringUtil::Format("%s/invoke/media/",
			Root::Instance()->getResCachePath().c_str());
		const String outPatGameRes = StringUtil::Format("%s/invoke/game/",
			Root::Instance()->getResCachePath().c_str());
		for (auto md5 : cacheFiles)
		{
			auto originName = md5ToNameMap[md5];
			String outFile;
			if (originName.find("/") == String::npos) //media resource
			{
				outFile = StringUtil::Format("%s%s", outPathMediaRes.c_str(), originName.c_str());
			}
			else //game resource
			{
				outFile = StringUtil::Format("%s%s", outPatGameRes.c_str(), originName.c_str());
			}

			auto cacheFile = StringUtil::Format("%s/%s",
				Root::Instance()->getResCachePath().c_str(), md5.c_str());
			PathUtil::CopyFileOrDir(cacheFile, outFile);
		}

#endif
	}
}


