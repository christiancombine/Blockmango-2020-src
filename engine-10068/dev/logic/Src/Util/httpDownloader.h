#pragma once
#ifndef __HTTP_DOWNLOADER_HEADER__
#define __HTTP_DOWNLOADER_HEADER__
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "cpr/cpr.h"

class HttpDownloader {

public:
	enum class ResultCode {
		SUCC = 0,
		CREATE_DIR_ERROR = 1,
		DOWNLOAD_DIRECTLY_NETWORK_ERROR = 2,
		DOWNLOAD_DIRECTLY_HTTP_ERROR = 3,
		DOWNLOAD_BY_RANGE_NETWORK_ERROR = 4,
		DOWNLOAD_BY_RANGE_HTTP_ERROR = 5,
		RECV_CONTENT_LENGTH_ERROR = 6,
		SAVE_TO_STORAGE_ERROR = 7,
		AFTER_SAVED_TO_STORAGE_CALLBACK_ERROR = 8
	};

	class Callbacker {
	public:
		// callbacks called in caller thread
		virtual void onProgress(int progress, uint32_t fileSize) = 0;
		virtual void onFail(ResultCode resultCode, const std::string& reason) = 0;
		virtual void onUnzipMap() = 0;
		virtual void onCheckMd5() = 0;
		virtual void onSucc() = 0;

		// callback called in download thread
		virtual bool afterSavedToStorage(const LORD::String& filePath) = 0;
	};

	struct DownloadStatus {
		std::recursive_mutex mutex;
		uint32_t downloadedBytes = 0;
		std::string downloadedContent;
		bool done = false;
	};

	struct DownloadData {
		std::thread downloadThread;
		uint32_t rangeBegin = 0;
		uint32_t rangeEnd = 0;
		uint32_t needDownloadBytes = 0;

		DownloadStatus status;
		~DownloadData() {
			try
			{
				if (downloadThread.joinable()) {
					downloadThread.detach();
				}
			}
			catch (...)
			{
				LordLogError("HttpDownloader, destruct and detach manage thread fail.");
			}
		}
	};

	struct Result {
		ResultCode code = ResultCode::SUCC;
		std::string reason;
		cpr::Error cprError;
		int32_t httpStatusCode = 0;

		Result() {
			code = ResultCode::SUCC;
			reason = "";
			cprError = cpr::Error();
			httpStatusCode = 0;
		}

		Result(ResultCode c, std::string r) {
			code = c;
			reason = r;
		}
	};

	struct DownloadSummary {
		std::recursive_mutex mutex;
		int totalProgress = 0; // -1: fail, [0,100]:downloading, 101: succ
		uint32_t totalDownloadedBytes = 0;
		Result result;
		std::string resultReason;
	};

	static constexpr int HTTP_CONNECT_TIMEOUT = 6000; //ms
	static constexpr int HTTP_TIMEOUT = 30000; //ms

public:
	HttpDownloader(const std::string& url, const std::string& saveDir, const std::string& fileName, 
		Callbacker* callbacker, uint32_t threadNum = 4, uint32_t maxRetryCnt = 1);

	virtual ~HttpDownloader();

	void onUnzipMap() { m_isOnUnzipMap = true; };
	void onCheckMd5() { m_isOnCheckMd5 = true; };

	bool finished();
	void tick();

private:
	void manageThreadFunc();
	void downloadThreadFunc(int index);
	void downloadDirectly();

	bool prepareSaveDir();
	void setResult(const HttpDownloader::Result& result);
	void startDownloadThreads();
	void downloadByRange();
	void saveToStorage(const std::string& content);
	void mergeAndSaveToStorage();
	uint32_t fetchContentLength(cpr::Header header);
	bool checkRangeSupport();

private:
	// members that write once and read only
	std::string m_url = "";
	std::string m_saveDir = "";
	std::string m_fileName = "";
	Callbacker* m_callbacker;
	uint32_t m_maxTryCnt = 2;

private:
	std::atomic<uint32_t> m_threadNum{ 0 };
	std::atomic<bool> m_callbackFinished{ false };
	std::atomic<bool> m_cleanUpFinished{ false };
	std::atomic<bool> m_isOnUnzipMap{ false };
	std::atomic<bool> m_isOnCheckMd5{ false };
	std::atomic<uint32_t> m_contentLength{ 0 };
	std::thread m_manageThread;
	DownloadSummary m_downloadSummary;
	std::vector<std::unique_ptr<DownloadData>> m_downloadDataVector;
};

#endif

