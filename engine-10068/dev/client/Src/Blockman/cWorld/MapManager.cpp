#include "MapManager.h"
#include <fstream>
#include "cpr/cpr.h"
#include "Resource/ZipArchive.h"
#include "libzippp.h"
#include "Util/ChecksumUtil.h"
#include "Util/Concurrent/WorkerThread.h"
#include "Util/PathUtil.h"
#include "Object/Root.h"
#include "Util/ClientEvents.h"
#include "Util/ClientDataReport.h"
#include "game.h"

#define RETURN_ON_FAIL(expr, msg, ...) if (!(expr)) { LordLogError(msg, ##__VA_ARGS__); return false; }

namespace GameClient
{

	const char* MapManager::DOWNLOAD_ADDRESS = "http://cloudatlasoft.u.qiniudn.com/";

	MapManager::MapManager()
	{
		m_mapRootPath = PathUtil::ConcatPath(Root::Instance()->getMapPath());
	}

	String MapManager::getMapPath(const String& mapName) const
	{
		return PathUtil::ConcatPath(getMapRootPath(), mapName);
	}

	bool MapManager::mapExistsAndValid(const String & mapName, const std::string& mapNameReal)
	{
		LordLogInfo("mapExistsAndValid, mapName:%s, mapNameReal:%s", mapName.c_str(), mapNameReal.c_str());

		// root dir is whether exist.
		String mapRootPath = getMapRootPath();
		LordLogInfo("mapExistsAndValid, mapRootPath:%s", mapRootPath.c_str());
		RETURN_ON_FAIL(PathUtil::IsDirExist(mapRootPath), "mapExistsAndValid root dir is missing : %s", mapRootPath.c_str());

		// mapNameId dir is whether exist.
		String mapIdPath = getMapPath(mapName);
		LordLogInfo("mapExistsAndValid, mapIdPath:%s", mapIdPath.c_str());
		RETURN_ON_FAIL(PathUtil::IsDirExist(mapIdPath), "mapExistsAndValid mapNameId dir is missing : %s", mapIdPath.c_str());

		// temp.zip is whether exist.
		String mapTempZipPath = PathUtil::ConcatPath(mapIdPath, (mapNameReal + "_temp.zip").c_str());
		LordLogInfo("mapExistsAndValid, mapTempZipPath:%s", mapTempZipPath.c_str());
		RETURN_ON_FAIL(!PathUtil::IsFileExist(mapTempZipPath), "mapExistsAndValid temp.zip is exist, should be broken-point continuingly-transferring : %s", mapTempZipPath.c_str());
		
		// map.zip is whether exist.
		String mapZipPath = PathUtil::ConcatPath(mapIdPath, (mapNameReal + ".zip").c_str());
		LordLogInfo("mapExistsAndValid, mapZipPath:%s", mapZipPath.c_str());
		RETURN_ON_FAIL(PathUtil::IsFileExist(mapZipPath), "mapExistsAndValid map is missing : %s", mapZipPath.c_str());

		// create map dir
		String mapPath = PathUtil::ConcatPath(mapIdPath, mapNameReal.c_str());
		LordLogInfo("mapExistsAndValid, mapPath:%s", mapPath.c_str());
		if (!PathUtil::IsDirExist(mapPath.c_str()) && !PathUtil::CreateDir(mapPath))
		{
			LordLogError("mapExistsAndValid failed to create map path %s", mapPath.c_str());
		}

		GameClient::CGame::Instance()->onProcessReport(BLOCKMAN::ClientDataReport::GAME_PROCESS_EVENT_UNZIP_MAP);

		// Unzip map.zip, then check md5
		libzippp::ZipArchive archive(mapZipPath.c_str());
		if (!archive.open())
		{
			PathUtil::DelPath(mapZipPath);
			LordLogError("mapExistsAndValid failed to open archive %s", mapZipPath.c_str());
			return false;
		}
		for (const auto& entry : archive.getEntries())
		{
			auto filePath = PathUtil::ConcatPath(mapPath, entry.getName().c_str());
			LordLogInfo("mapExistsAndValid, from archive,filePath:%s", filePath.c_str());
			if (entry.isDirectory())
			{
				if (!PathUtil::IsDirExist(filePath.c_str())) 
				{
					if (!PathUtil::CreateDir(filePath.c_str())) {
						LordLogError("mapExistsAndValid call PathUtil::CreateDir to create dir [%s] failed", filePath.c_str());
					}
				}
			}
			else
			{
				std::ofstream os(filePath.c_str(), std::ios::binary | std::ios::trunc);
				entry.readContent(os);
			}
		}

		int closeRet = archive.close();
		if (LIBZIPPP_OK != closeRet) {
			LordLogError("mapExistsAndValid call archive.close failed, error code:%d", closeRet);
		}

		GameClient::CGame::Instance()->onProcessReport(BLOCKMAN::ClientDataReport::GAME_PROCESS_EVENT_MD5_CHECK);
		// check md5
		RETURN_ON_FAIL(checkMd5(mapPath), "checksums.md5 error : %s", mapNameReal.c_str());
		return true;
	}

	bool MapManager::checkMd5(const String & mapPath)
	{
		String checksumPath = PathUtil::ConcatPath(mapPath, "checksums.md5");
		RETURN_ON_FAIL(PathUtil::IsFileExist(checksumPath), "checksums.md5 is missing in the map %s", mapPath.c_str());
		RETURN_ON_FAIL(BLOCKMAN::ChecksumUtil::verify(checksumPath), "map %s failed the checksum verification", mapPath.c_str());
		StringArray fileList;
		PathUtil::EnumFilesInDir(fileList, mapPath, false, true);
		int nEntryCount = BLOCKMAN::ChecksumUtil::getEntryCount(checksumPath);
		RETURN_ON_FAIL(fileList.size() == nEntryCount + 1, "number of files in map %s is incorrect:%d %d",
			mapPath.c_str(), fileList.size(), nEntryCount);

		return true;
	}

	void MapManager::delelteMap(const String & mapName, const String & mapUrl)
	{
		// parse mapNameReal from mapUrl
		std::string mapUrlStr(mapUrl.c_str());
		std::string mapNameReal("");

		if (mapUrlStr.find_last_of("/") != mapUrlStr.npos)
		{
			mapNameReal = mapUrlStr.substr(mapUrlStr.find_last_of("/") + 1);
		}

		if (mapNameReal.find(".zip") != mapNameReal.npos)
		{
			mapNameReal = mapNameReal.substr(0, mapNameReal.find(".zip"));
		}

		String mapIdPath = getMapPath(mapName);
		String mapPath = PathUtil::ConcatPath(mapIdPath, mapNameReal.c_str());
		LordLogInfo("MapManager::delelteMap begin, mapPath:%s, mapIdPath:%s, mapNameReal:%s", 
			mapPath.c_str(), mapIdPath.c_str(), mapNameReal.c_str());

		if (PathUtil::IsDirExist(mapPath))
		{
			PathUtil::DelPath(mapPath);
			LordLogInfo("MapManager::delelteMap succ, mapPath:%s", mapPath.c_str());
		}
	}

	// callbacks called in caller thread
	void MapManager::onProgress(int progress, uint32_t fileSize) {
		BLOCKMAN::MapManagerDownloadProgress::emit(progress, fileSize);
	}

	void MapManager::onFail(HttpDownloader::ResultCode resultCode, const std::string& reason) {
		if (!m_firstFailed) {
			m_firstFailed = true;
		}

		BLOCKMAN::MapManagerDownloadFail::emit((int)resultCode, reason, m_hasRetried);
	}

	void MapManager::onUnzipMap()
	{
		GameClient::CGame::Instance()->onProcessReport(BLOCKMAN::ClientDataReport::GAME_PROCESS_EVENT_UNZIP_MAP);
	}

	void MapManager::onCheckMd5()
	{
		GameClient::CGame::Instance()->onProcessReport(BLOCKMAN::ClientDataReport::GAME_PROCESS_EVENT_MD5_CHECK);
	}

	void MapManager::onSucc() {
		BLOCKMAN::MapManagerDownloadSucc::emit();
	}

	// callback called in download thread
	bool MapManager::afterSavedToStorage(const LORD::String& filePath) {
		// create map dir
		if (!PathUtil::IsDirExist(m_mapPath) && !PathUtil::CreateDir(m_mapPath)) {
			LordLogError("MapManager::afterSavedToStorage, failed to create map path %s", m_mapPath.c_str());

			return false;
		}

		m_downloader->onUnzipMap();

		// unzip map
		libzippp::ZipArchive archive(filePath.c_str());
		if (!archive.open())
		{
			PathUtil::DelPath(filePath);
			LordLogError("MapManager::afterSavedToStorage, failed to open zip file: %s", filePath.c_str());

			return false;
		}
		for (const auto& entry : archive.getEntries())
		{
			auto fpath = PathUtil::ConcatPath(m_mapPath, entry.getName().c_str());
			if (entry.isDirectory())
			{
				if (!PathUtil::IsDirExist(fpath.c_str())) {
					PathUtil::CreateDir(fpath.c_str());
				}
			}
			else
			{
				std::ofstream os(fpath.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
				int readResult = entry.readContent(os);
				if (LIBZIPPP_OK != readResult) {
					LordLogError("MapManager::afterSavedToStorage, call entry.readContent(os) failed, error code:%d", readResult);
					break;
				}
				os.flush();
			}
		}
		
		int closeResult = archive.close();
		if (LIBZIPPP_OK != closeResult) {
			LordLogError("MapManager::afterSavedToStorage, call archive.close failed, error code:%d", closeResult);
			return false;
		}

		m_downloader->onCheckMd5();

		// check md5
		if (!checkMd5(m_mapPath)) {
			LordLogError("MapManager::afterSavedToStorage, checksums.md5 error, m_mapPath: %s", m_mapPath.c_str());
			return false;
		}

		return true;
	}

	bool MapManager::downloadMap(const String & mapId, const String& mapUrl, const String& mapNameReal)
	{
		if (m_downloader) {
			return false;
		}

		m_firstFailed = false;
		m_hasRetried = false;

		m_saveDir = getMapPath(mapId);
		m_mapPath = PathUtil::ConcatPath(m_saveDir, mapNameReal);
		m_fileName = mapNameReal + ".zip";

		m_url = mapUrl;
		m_downloader = std::make_shared<HttpDownloader>(m_url.c_str(), m_saveDir.c_str(), m_fileName.c_str(), this);

		return true;
	}

	void MapManager::tick() {
		if (!m_downloader) {
			return;
		}

		if (!m_downloader->finished()) {
			m_downloader->tick();
		} 
		else {
			if (m_firstFailed) {
				if (!m_hasRetried) {
					m_hasRetried = true;
					m_downloader = std::make_shared<HttpDownloader>(m_url.c_str(), m_saveDir.c_str(), m_fileName.c_str(), this);
				}
				else {
					m_downloader.reset();
				}
			}
			else {
				m_downloader.reset();
			}
		}
	}
}
