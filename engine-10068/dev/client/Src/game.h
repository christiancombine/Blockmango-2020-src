#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include "Core.h"
#include "curl.h"

#include "Network/ClientNetwork.h"
#include "Util/SubscriptionGuard.h"

namespace LORD
{
	class SoundEngine;
}

namespace BLOCKMAN
{
	class CSVReader;
	enum class ClientGameType;
	class Blockman;
}

namespace GameClient
{
	class Scheduler;
	class ShellInterface;

	struct GameConfig
	{
		LORD::String	updateServerAddress;
		LORD::String	versionFileName;
		LORD::String	fileListName;
		LORD::StringArray serverList;
		size_t			nCurrentServerIndex;

		GameConfig()
		{
			updateServerAddress = "";
			versionFileName = "";
			fileListName = "";
			nCurrentServerIndex = 0;
		}
	};

	struct updateFileData
	{
		LORD::String	filename;
		LORD::String	path;
		LORD::String	md5;
		bool			isPak;
		int				size;
		// 0 no update
		// 1 add
		// 2 modify
		// 3 delete
		int				operate;
	};

	typedef LORD::map<LORD::String, updateFileData>::type FILE_LIST_TYPE;

	class CGame : public LORD::Singleton<CGame>
	{
	public:
		CGame();
		virtual ~CGame();

		static CGame* Instance();

		void InitConfig(const char* strRootPath);
		void LoadSetting();
		void UnloadSetting();

		void InitGame(const char* strRootPath, const char* strConfigPath, int width, int height, unsigned int hWnd, LORD::SoundEngine* pSoundEngine = NULL, const char* strMapPath = NULL);
		
		void InitGame(const char* nickName, ui64 userId, const char* token, const char* ip, int port, ui64 gameTimestamp, 
			const char* lang, const char* gameType, const char* mapId, const char* mapUrl, const char* strRootPath, 
			const char* strConfigPath, int width, int height, unsigned int hWnd, const char* dispatchRequestId,
			LORD::SoundEngine* pSoundEngine = NULL, const char* strMapPath = NULL, bool isNewStartMode = false, bool enterGameWithTeam = false);

		bool onDownloadMapProgress(int progress, uint32_t fileSize);
		bool onDownloadMapFailure(int failureCode, const std::string& failureMessage, bool hasRetried);
		bool onDownloadMapSuccess();

		void connectServer(const char* ip, int port, const char * dispatchRequestId);
		
		void loadMapDynamicRes();

		bool retryConnect(bool isChangeIp);

		void DestroyGame();

		void useProp(const char * propId);
		int getPing();
		int getRaknetPing();
		void setPing(int ping) { m_ping = ping; }
		void setReceivePingTime(ui32  receivePingTime) {
			this->m_receivePingTime = receivePingTime; 
			this->m_receivedPingAck = true;
		}
		void setSendLoginTime(ui32 sendLoginTime) { this->m_sendLoginTime = sendLoginTime; }

		void setDbDataReady() { m_dbDataReady = true; }
		bool isDbDataReady() { return m_dbDataReady; }

		void dataReport(String mainEvent);
		void dataReport(String mainEvent, String childEvent);
		void dataReportNotType(String mainEvent, String childEvent);
		void reportGLESVersion();
		void onError(int errorCode);
		void onFriendOperationForAppHttpResult(int operationType, ui64 userId);
		void setGameType(String gameType);
		bool exitGame(String params = "normal");
		void retryEnter(int code, String message);
		void resetGame(String dispatchInfo);
		void resetGame(ui64 targetId);
		void resetGame(String gameType, ui64 targetId, String mapId = "");
		void resetGameAddr(ui64 targetId, String gameType, String mapId, String addr);
		void resetGameResult(int result);// 1 succeed 2 call on manor failure ... other result undefined
		void MainTick();
		void onRechargeResult(int type, int result);
		void buyActionResult(int type, int result);
		void addAsyncCall(std::function<void(void)> fuc) { m_clientAsyncCall.noblockCall(fuc); };
		void setUserToken(const char * token) { m_userToken = String(token); }
		void setBaseUrl(const char * url) { m_baseUrl = String(url); }
		void setPicUrl(String url) { m_picUrl = url; }
		void setInviter(i64 inviter) { m_inviter = inviter; }
		i64 getInviter() { return m_inviter; }
		void onExpResult(int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status);
		void onWatchAdResult(int type, String params, int code);
		void onGameActionTrigger(int type, String params = "");

		void OnSize(int nWidth, int nHeight);

		// 		void OnTouchDown(int nIndex, float fPositionX, float fPositionY);
		// 
		// 		void OnTouchUp(int nIndex, float fPositionX, float fPositionY);
		// 
		// 		void OnTouchMove(int nIndex, float fPositionX, float fPositionY);

		void handleTouchClick(float x, float y);

		bool handleTouchesBegin(int num, intptr_t ids[], float xs[], float ys[]);
		bool handleTouchesMove(int num, intptr_t ids[], float xs[], float ys[]);
		bool handleTouchesEnd(int num, intptr_t ids[], float xs[], float ys[]);
		bool handleTouchesCancel(int num, intptr_t ids[], float xs[], float ys[]);

		
		void handleKeyDown(int key);
		void handleKeyUp(int key);
		void handleMouseMove(int x, int y);

		void handleMotionBegin();
		void handleMotionCancelled();
		void handleMotionEnded();

		
		void OnPause();

		
		void OnResume();

		void SetMaxFps(unsigned int fps)
		{
#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
			m_fps = fps;
			maxFrameTime = 1000 / m_fps;
#endif
		}

		//void OnTouchRotateAndScale(float centerX, float centerY, float touchX, float touchY, float startX, float startY);

		int	CheckVersion(const char* strRootPath);

		int UpdateFiles();

		float GetUpdatePercent();

		int	GetTotalDownloadSize() { return m_totalUpdateSize; }
		int	GetCurrentDownloadSize() { return m_curDownloadSize; }

		LORD::String getLocalVersion() { return m_localVersion; }
		LORD::String getServerVersion() { return m_serverVersion; }

		int	GetUpdateState() const { return m_updateState; }

		void setShellInterface(ShellInterface* shellInterface) { m_shellInterface = shellInterface; }

		ShellInterface* getShellInterface() { return m_shellInterface; }

		void GetPhoneType(const char* strPhoneType);

		const LORD::String& GetPhoneInfo() const;
		const LORD::String& GetGPUInfo() const;

		void setPlatformInfo(const char* platformInfo) { m_platformInfo = platformInfo; }

		const char* getPlatformInfo() { return m_platformInfo.c_str(); }

		void    printLuaStack();

		void  setGetServerInfo(bool get) { m_hasGetServerInfo = get; }
		bool  hasGetServerInfo() { return m_hasGetServerInfo; }

		String getPlayerName() { return m_playerName; }
		String getRootPath() { return m_rootPath; }
		String getConfigPath() { return m_configPath; }

		ui64 getPlatformUserId() { return m_platformUserId; }
		ui64 getGameTimestamp() { return m_gameTimestamp; }
		String getGameToken() { return m_gameToken; }
		String getUserToken() { return m_userToken;  }
		String getBaseUrl() { return m_baseUrl; }
		String getLanguage() { return m_lang; }
		String getGameType() { return m_gameType; }
		String getPicUrl() { return m_picUrl; }
		String getDispatchRequestId() { return m_dispatchRequestId; }
		void setDisplayDensity(float displayDensity) { this->m_displayDensity = displayDensity; }
		float getDisplayDensity() { return m_displayDensity; }

		std::shared_ptr<ClientNetwork> getNetwork() { return m_network; }

		void toggleDebugMessageShown(bool shown) { m_debugMessageShown = shown; }
		bool isDebugMessageShown() { return m_debugMessageShown; }

		BLOCKMAN::ClientGameType GetGameType();
		float getFrameTime();

		bool isEnterBack() { return m_enterBackFlag; }
		bool isEnableIndie() { return m_enableIndie; }
		void enableIndie() { m_enableIndie = true; }

		void hideRechargeBtn();

		void	recordRecvedLoginResultTime();
		ui32	getRecvedLoginResultTime() {
			return m_recvedLoginResultTime;
		}

		bool clientOnGroundCheatDetected();
		void reportEnterGameTime();
		void onProcessReport(LORD::String event);

		bool enterGameWithTeam() { return m_enterGameWithTeam; }

		int getFps() { return m_avgFps > 0 ? m_avgFps : -1; }

	protected:

		// update fun
		int DownloadAllFiles(const char* strRootPath);

		CURLcode DownloadFile(const char* strRootPath, const char* fileName, long timeout, const char* md5);

		int CompareFilelist(const char* oldlist, const char* newlist);

		int ReadFilelist(const char* filelistPath, FILE_LIST_TYPE& filelist);

		LORD::String GetUpdateTempDir(const char* strRootPath);

		int CopyAllFiles(const LORD::String& srcRoot, const LORD::String& dstRoot);

		// tick
		void RetryNetworkConnection();

		void GameLogicTick();

		void RenderTick();


		bool	m_bInited;
		bool    m_enterBackFlag;

		void	downloadMap();

		std::string timeToStepStr(ui32 time);
		std::string generateEnterGameTimeStr();

		// update log
		void outputLogFormate(FILE* pFile, const char* formate, ...)
		{
			if (pFile)
			{
				char midresult[1024];
				memset(midresult, 0, 1024);
				va_list args;

				va_start(args, formate);
				snprintf(midresult, 1024, formate, args);
				LORD::String result = midresult;
				va_end(args);

				fwrite(result.c_str(), result.size(), 1, pFile);
			}
		}

	private:
		void loadScriptRes();
		void loadScript();
		void unloadScript();
		void resetGame();
		void updateTestStartParam();

	private:
		CURL*		m_easyDownloadHandle = nullptr;
		GameConfig	m_gameConfig;

		FILE_LIST_TYPE	m_oldfilelist;
		FILE_LIST_TYPE	m_newfilelist;
		LORD::vector<updateFileData>::type				m_needUpdatefile;

		unsigned int        m_fps = 0;
		unsigned int		maxFrameTime = 0;

		LORD::String		m_nickName = "";
		ui64				m_platformUserId = 0;
		LORD::String		m_gameToken = "";
		LORD::String		m_gameIP = "";
		int					m_gamePort = 0;
		ui64				m_gameTimestamp = 0;
		LORD::String		m_lang = "";
		LORD::String		m_mapId = "";
		LORD::String		m_mapUrl = "";
		LORD::String		m_rootPath = "";
		LORD::String		m_configPath = "";
		LORD::String		m_mapPath = "";
		LORD::String		m_localVersion = "";
		LORD::String		m_serverVersion = "";

		int					m_width = 0;
		int					m_height = 0;
		unsigned int		m_hWnd = 0;
		LORD::String		m_dispatchRequestId = "";
		LORD::SoundEngine*	m_soundEngine = nullptr;
		int					m_needUpdateNum = 0;
		int					m_curUpdateIndex = 0;
		int					m_totalUpdateSize = 0;
		int					m_curDownloadSize = 0;

		BLOCKMAN::Blockman* m_blockman = nullptr;

		FILE*				m_updateLogHandle = nullptr;
		int					m_updateState = 0;

		ShellInterface*		m_shellInterface = nullptr;

		LORD::String		m_platformInfo = "";

		std::shared_ptr<ClientNetwork>      m_network;

		bool                m_hasGetServerInfo = false;

		String              m_playerName = "";

		ui64                m_tickCount = 0;

		char                m_ip[20] = "";
		ui16                m_port = 0;

		bool                m_bNetGame = false;
		float		m_displayDensity = 1.0f;

		int			m_ping = 0;
		bool                m_debugMessageShown;
		ui32                m_receivePingTime = 0;
		bool				m_receivedPingAck = true;
		ui32			m_sendLoginTime = 0;
		bool		m_dbDataReady = false;
		String		m_errorCode = "";
		String		m_gameType = "";
		String		m_userToken = "";
		String		m_baseUrl = "";
		String		m_picUrl = "";
		i64			m_inviter = 0;
		BLOCKMAN::SubscriptionGuard m_subscriptionGuard;
		SPSCAsyncCall m_clientAsyncCall;
		i32			m_retryEnterTimes = 0;
		bool		m_enableIndie = false;
		bool		m_loadCommonPluginFinished = false;

		std::string m_mapNameReal;

		// report game over related
		ui32	  m_initGameTime = 0;
		ui32	  m_mapStartDownloadTime = 0;
		ui32	  m_mapDownloadedTime = 0;
		ui32      m_recvedLoginResultTime = 0;

		//
		bool m_isNewStartMode = false;
		
		bool m_enterGameWithTeam = false;

		bool m_gameDestroyedBefore = false;

		bool m_isReseting = false;

		bool needReportProcess = true;

		int m_avgFps;

		ui64 m_avgFpsCount = 0;

		map<int, Color>::type m_fogCustomColor{
			{0, Color(0.000f, 0.000f, 0.000f)},
			{1, Color(0.000f, 0.000f, 0.000f)},
			{2, Color(0.000f, 0.000f, 0.000f)},
			{3, Color(0.000f, 0.000f, 0.000f)},
			{4, Color(0.000f, 0.000f, 0.000f)},
			{5, Color(0.000f, 0.000f, 0.000f)},
			{6, Color(0.131f, 0.145f, 0.224f)},
			{7, Color(0.327f, 0.392f, 0.509f)},
			{8, Color(0.264f, 0.346f, 0.705f)},
			{9, Color(0.200f, 0.300f, 0.900f)},
			{10, Color(0.200f, 0.300f, 0.900f)},
			{11, Color(0.200f, 0.300f, 0.900f)},
			{12, Color(0.200f, 0.300f, 0.900f)},
			{13, Color(0.200f, 0.300f, 0.900f)},
			{14, Color(0.200f, 0.300f, 0.900f)},
			{15, Color(0.200f, 0.300f, 0.900f)},
			{16, Color(0.200f, 0.300f, 0.900f)},
			{17, Color(0.200f, 0.300f, 0.900f)},
			{18, Color(0.200f, 0.300f, 0.900f)},
			{19, Color(0.200f, 0.300f, 0.900f)},
			{20, Color(0.478f, 0.336f, 0.616f)},
			{21, Color(0.715f, 0.372f, 0.332f)},
			{22, Color(0.000f, 0.000f, 0.000f)},
			{23, Color(0.000f, 0.000f, 0.000f)}
		};
	};
}

#endif