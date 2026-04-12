#ifndef __MAP_MANAGER_HEADER__
#define __MAP_MANAGER_HEADER__

#include <atomic>
#include "Core.h"

#include "Util/httpDownloader.h"

using namespace LORD;

namespace GameClient
{
	class MapManager : public Singleton<MapManager>, public ObjectAlloc, public HttpDownloader::Callbacker
	{
	private:
		String m_url;
		String m_saveDir;
		String m_fileName;

		String m_mapPath;

		bool m_firstFailed = false;
		bool m_hasRetried = false;

		std::shared_ptr<HttpDownloader> m_downloader;

		String m_mapRootPath;
		static const char* DOWNLOAD_ADDRESS;
		
	public:
		MapManager();

		String getMapRootPath() const
		{
			return m_mapRootPath;
		}

		String getMapPath(const String& mapName) const;

		bool mapExistsAndValid(const String& mapName, const std::string& mapNameReal);
		bool checkMd5(const String& mapPath);
		void delelteMap(const String& mapName, const String& mapUrl);

		void tick();

	public:
		bool downloadMap(const String& mapId, const String& mapUrl, const String& mapNameReal);

		// callbacks called in caller thread
		virtual void onProgress(int progress, uint32_t fileSize);

		virtual void onFail(HttpDownloader::ResultCode resultCode, const std::string& reason);


		virtual void onUnzipMap();

		virtual void onCheckMd5();

		virtual void onSucc();

		// callback called in download thread
		virtual bool afterSavedToStorage(const LORD::String& filePath);
	};
}

#endif // !__MAP_MANAGER_HEADER__
