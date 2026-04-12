#include <algorithm>
#include <chrono>
#include <sstream>
#include "Core.h"
#include "Util/httpDownloader.h"
#include "Util/PathUtil.h"


HttpDownloader::HttpDownloader(const std::string& url, const std::string& saveDir, const std::string& fileName,
	Callbacker* callbacker, uint32_t threadNum, uint32_t maxRetryCnt)
{
	m_url = url;
	m_saveDir = saveDir;
	m_fileName = fileName;
	m_callbacker = callbacker;

	if (threadNum <= 0) {
		threadNum = 1;
	}
	m_threadNum = threadNum;

	m_maxTryCnt = 1 + maxRetryCnt;

	m_manageThread = std::thread(&HttpDownloader::manageThreadFunc, this);
}

HttpDownloader::~HttpDownloader()
{
	try
	{
		if (m_manageThread.joinable()) {
			m_manageThread.detach();
		}
	}
	catch (...)
	{
		LordLogError("HttpDownloader, destruct and detach manage thread fail.");
	}
}

void HttpDownloader::setResult(const HttpDownloader::Result& result)
{
	std::lock_guard<std::recursive_mutex> lock(m_downloadSummary.mutex);
	if (m_downloadSummary.result.code != ResultCode::SUCC) {
		return; // remain the first error
	}

	m_downloadSummary.result = result;
}

void HttpDownloader::tick() {
	if (m_callbackFinished) {
		return;
	}

	static int lastTotalProgress = 0;
	int totalProgress = 0;
	ResultCode resultCode = ResultCode::SUCC;

	std::string reason;
	{
		std::lock_guard<std::recursive_mutex> lock(m_downloadSummary.mutex);
		totalProgress = m_downloadSummary.totalProgress;
		resultCode = m_downloadSummary.result.code;
		reason = m_downloadSummary.result.reason;
	}

	if (totalProgress < lastTotalProgress) {
		totalProgress = lastTotalProgress;
	}

	if (m_isOnUnzipMap)
	{
		m_isOnUnzipMap = false;
		m_callbacker->onUnzipMap();
	}

	if (m_isOnCheckMd5)
	{
		m_isOnCheckMd5 = false;
		m_callbacker->onCheckMd5();
	}

	if (resultCode != ResultCode::SUCC) {
		m_callbacker->onFail(resultCode, reason);
		m_callbackFinished = true;
	} else if (totalProgress > 100) {
		m_callbacker->onSucc();
		m_callbackFinished = true;
	} else {
		if (totalProgress > lastTotalProgress) {
			m_callbacker->onProgress((std::min)(totalProgress, 99), m_contentLength);
		}
	}

	lastTotalProgress = (std::min)(totalProgress, 99);
}

bool HttpDownloader::finished() {
	return m_callbackFinished && m_cleanUpFinished;
}

void HttpDownloader::downloadThreadFunc(int index) {
	int tryCnt = 0;
	HttpDownloader::Result downloadResult;

	while (tryCnt < m_maxTryCnt) {
		tryCnt++;
		downloadResult = HttpDownloader::Result();
		
		{
			std::lock_guard<std::recursive_mutex> lock(m_downloadDataVector[index]->status.mutex);
			m_downloadDataVector[index]->status.downloadedBytes = 0;
			m_downloadDataVector[index]->status.downloadedContent = "";

			if (m_downloadDataVector[index]->status.done) {
				break;
			}
		}

		std::stringstream ss;
		ss << "bytes=" << m_downloadDataVector[index]->rangeBegin << "-" << m_downloadDataVector[index]->rangeEnd;
		std::string rangeStr = ss.str();

		cpr::Response response = cpr::Get(
			cpr::Url(m_url.c_str()),
			cpr::ConnectTimeout(std::chrono::milliseconds(HttpDownloader::HTTP_CONNECT_TIMEOUT)),
			cpr::Timeout(std::chrono::milliseconds(HttpDownloader::HTTP_TIMEOUT)),
			cpr::Header{ { "Range", rangeStr } },
			cpr::XferInfo([&](const cpr::TransferProgress& progress) -> int
		{
			std::lock_guard<std::recursive_mutex> lock(m_downloadDataVector[index]->status.mutex);
			m_downloadDataVector[index]->status.downloadedBytes = static_cast<uint32_t>(progress.downloadNow);
			
			return 0;
		}));

		if (response.error.code != cpr::ErrorCode::OK) {
			downloadResult = HttpDownloader::Result(HttpDownloader::ResultCode::DOWNLOAD_BY_RANGE_NETWORK_ERROR, "network request fail");
			downloadResult.cprError = response.error;
			LordLogError("HttpDownloader, download by range[%d] failed, response.error.code = %d", index, static_cast<int>(response.error.code));

			continue;
		}

		if (response.status_code != 200 && response.status_code != 206) {
			downloadResult = HttpDownloader::Result(HttpDownloader::ResultCode::DOWNLOAD_BY_RANGE_HTTP_ERROR, "http request fail");
			downloadResult.httpStatusCode = response.status_code;
			LordLogError("HttpDownloader, download by range[%d] failed, httpStatusCode = %d", index, response.status_code);

			continue;
		}

		if (response.text.length() < m_downloadDataVector[index]->needDownloadBytes) {
			downloadResult = HttpDownloader::Result(HttpDownloader::ResultCode::DOWNLOAD_BY_RANGE_HTTP_ERROR,
				"response.text.length() < needDownloadBytes");

			LordLogError("HttpDownloader, download by range[%d] failed, need %u, only download %u", index,
				m_downloadDataVector[index]->needDownloadBytes, response.text.length());

			continue;
		}

		{
			std::lock_guard<std::recursive_mutex> lock(m_downloadDataVector[index]->status.mutex);
			m_downloadDataVector[index]->status.downloadedContent = response.text;
		}

		break;
	}

	if (downloadResult.code != ResultCode::SUCC) {
		setResult(downloadResult);

		return;
	}

}

void HttpDownloader::startDownloadThreads() {
	uint32_t maxThreadNum = static_cast<uint32_t>(std::ceil(m_contentLength / 100) + 1);
	if (m_threadNum > maxThreadNum) {
		m_threadNum = maxThreadNum;
	}

	uint32_t segmentLength = static_cast<uint32_t>(std::ceil(m_contentLength / m_threadNum) + 1);
	m_downloadDataVector.reserve(m_threadNum);

	for (uint32_t i = 0; i < m_threadNum; ++i) {
		std::unique_ptr<DownloadData>  ddata(new DownloadData);
		ddata->rangeBegin = static_cast<uint32_t>(i * segmentLength);
		ddata->rangeEnd = static_cast<uint32_t>((i + 1) * segmentLength) - 1;
		if (i + 1 == m_threadNum) {
			ddata->rangeEnd = m_contentLength - 1;
		}
		ddata->needDownloadBytes = ddata->rangeEnd - ddata->rangeBegin + 1;
	
		m_downloadDataVector.push_back(std::move(ddata));
	}

	for (uint32_t i = 0; i < m_threadNum; ++i) {
		m_downloadDataVector[i]->downloadThread = std::thread(&HttpDownloader::downloadThreadFunc, this, i);

		LordLogInfo("HttpDownloader, start thread[%u], %u-%u", i, m_downloadDataVector[i]->rangeBegin, m_downloadDataVector[i]->rangeEnd);
	}
}

void HttpDownloader::downloadByRange() {
	startDownloadThreads();

	uint32_t totalBytes = 0;
	bool readyToMerge = true;
	while (true) {
		totalBytes = 0;
		readyToMerge = true;

		for (uint32_t i = 0; i < m_threadNum; ++i) {
			std::lock_guard<std::recursive_mutex> lock(m_downloadDataVector[i]->status.mutex);
			totalBytes += m_downloadDataVector[i]->status.downloadedBytes;
			if (m_downloadDataVector[i]->status.downloadedContent.length() < m_downloadDataVector[i]->needDownloadBytes) {
				readyToMerge = false;
			}
		}

		{
			std::lock_guard<std::recursive_mutex> lock(m_downloadSummary.mutex);

			if (m_downloadSummary.result.code != ResultCode::SUCC) {
				break;
			}

			m_downloadSummary.totalDownloadedBytes = totalBytes;
			m_downloadSummary.totalProgress = static_cast<int>(m_downloadSummary.totalDownloadedBytes * 100 / m_contentLength);

		}

		if (totalBytes >= m_contentLength && readyToMerge) {
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	{
		std::lock_guard<std::recursive_mutex> lock(m_downloadSummary.mutex);

		if (m_downloadSummary.result.code == ResultCode::SUCC) {
			if (totalBytes >= m_contentLength && readyToMerge) {
				mergeAndSaveToStorage();
			}
			else {
				setResult(HttpDownloader::Result(ResultCode::RECV_CONTENT_LENGTH_ERROR, "totalBytes < m_contentLength"));
			}
		}
	}


	for (uint32_t i = 0; i < m_threadNum; ++i) {
		std::lock_guard<std::recursive_mutex> lock(m_downloadDataVector[i]->status.mutex);
		m_downloadDataVector[i]->status.done = true;
	}

	for (uint32_t i = 0; i < m_threadNum; ++i) {
		m_downloadDataVector[i]->downloadThread.join();
	}
}

void HttpDownloader::downloadDirectly() {
	HttpDownloader::Result downloadResult;
	cpr::Response response;

	int tryCnt = 0;
	while (tryCnt < m_maxTryCnt) {
		tryCnt++;
		downloadResult = HttpDownloader::Result();

		response = cpr::Get(
			cpr::Url(m_url.c_str()),
			cpr::ConnectTimeout(std::chrono::milliseconds(HttpDownloader::HTTP_CONNECT_TIMEOUT)),
			cpr::Timeout(std::chrono::milliseconds(HttpDownloader::HTTP_TIMEOUT)),
			cpr::XferInfo([&](const cpr::TransferProgress& progress) -> int
		{
			std::lock_guard<std::recursive_mutex> lock(m_downloadSummary.mutex);
			if (progress.downloadTotal == 0) {
				m_downloadSummary.totalProgress = 0;
			} else {
				m_downloadSummary.totalDownloadedBytes = static_cast<uint32_t>(progress.downloadTotal);
				m_downloadSummary.totalProgress = static_cast<int>(progress.downloadNow * 100 / progress.downloadTotal);

				m_contentLength = static_cast<uint32_t>(progress.downloadTotal);
			}

			return 0;
		}));

		if (response.error.code != cpr::ErrorCode::OK) {
			downloadResult = HttpDownloader::Result(ResultCode::DOWNLOAD_DIRECTLY_NETWORK_ERROR, "network request fail");
			downloadResult.cprError = response.error;
			LordLogError("HttpDownloader, downloadDirectly failed, response.error.code = %d", static_cast<int>(response.error.code));
			
			continue;
		}

		if (response.status_code != 200) {
			downloadResult = HttpDownloader::Result(ResultCode::DOWNLOAD_DIRECTLY_HTTP_ERROR, "http request fail");
			downloadResult.httpStatusCode = response.status_code;
			LordLogError("HttpDownloader, downloadDirectly failed, httpStatusCode = %d", response.status_code);

			continue;
		}

		break;
	}

	if (downloadResult.code != ResultCode::SUCC) {
		setResult(downloadResult);

		return;
	}


	saveToStorage(response.text);
}

void HttpDownloader::saveToStorage(const std::string& content) {
	int tryCnt = 0;
	HttpDownloader::Result saveResult;

	LORD::String filePath;
	while (tryCnt < m_maxTryCnt) {
		tryCnt++;
		saveResult = HttpDownloader::Result();

		filePath = LORD::PathUtil::ConcatPath(m_saveDir.c_str(), m_fileName.c_str());
		std::ofstream os(filePath.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
		if (!os || !os.write(content.data(), content.size()) || !os.flush()) {

			LORD::PathUtil::DelPath(filePath);

			saveResult = HttpDownloader::Result(ResultCode::SAVE_TO_STORAGE_ERROR, "write file failed");
			LordLogError("HttpDownloader, saveToStorage failed, filePath:%s, errno:%d, errmsg:%s", filePath.c_str(), errno, strerror(errno));
			
			continue;
		}
		os.close();

		break;
	}

	if (saveResult.code != ResultCode::SUCC) {
		setResult(saveResult);

		return;
	}

	if (!m_callbacker->afterSavedToStorage(filePath)) {
		saveResult = HttpDownloader::Result(ResultCode::AFTER_SAVED_TO_STORAGE_CALLBACK_ERROR, "onSavedToStorage callback error");
		setResult(saveResult);

		return;
	}

	{
		std::lock_guard<std::recursive_mutex> lock(m_downloadSummary.mutex);
		if (m_downloadSummary.result.code == ResultCode::SUCC) {
			m_downloadSummary.totalProgress = 101;
		}
	}
}

void HttpDownloader::mergeAndSaveToStorage() {
	uint32_t threadNum = m_threadNum.load();
	std::string content;
	for (uint32_t i = 0; i < threadNum; ++i) {
		std::lock_guard<std::recursive_mutex> lock(m_downloadDataVector[i]->status.mutex);
		content.append(m_downloadDataVector[i]->status.downloadedContent);
	}

	saveToStorage(content);
}

uint32_t HttpDownloader::fetchContentLength(cpr::Header header) {
	if (header.find("Content-Length") == header.end()) {
		return 0;
	}

	std::string contentLength = header["Content-Length"];
	if (contentLength.length() <= 0) {
		return 0;
	}

	return std::stoul(contentLength);
}

bool HttpDownloader::checkRangeSupport() {
	int tryCnt = 0;

	while (tryCnt < m_maxTryCnt) {
		tryCnt++;

		cpr::Response response = cpr::Head(
			cpr::Url(m_url.c_str()),
			cpr::ConnectTimeout(std::chrono::milliseconds(HttpDownloader::HTTP_CONNECT_TIMEOUT)),
			cpr::Timeout(std::chrono::milliseconds(HttpDownloader::HTTP_TIMEOUT)),
			cpr::Header{ { "Range", "bytes=0-" } });

		if (response.error.code != cpr::ErrorCode::OK) {
			continue;
		}

		if (response.status_code == 206 || response.status_code == 200) {
			uint32_t contentLength = fetchContentLength(response.header);
			if (contentLength <= 0) {
				continue;
			}

			m_contentLength = contentLength;

			return true;
		}
	}

	return false;
}

bool HttpDownloader::prepareSaveDir() {
	if (!LORD::PathUtil::IsDirExist(m_saveDir.c_str()))
	{
		if (!LORD::PathUtil::CreateDir(m_saveDir.c_str())) {
			std::string msg = "create dir fail:";
			msg.append(m_saveDir);
			HttpDownloader::Result result(ResultCode::CREATE_DIR_ERROR, msg);
			setResult(result);

			return false;
		}
	}

	return true;
}

void HttpDownloader::manageThreadFunc() {

	if (prepareSaveDir()) {
		bool supportRange = checkRangeSupport();
		if (supportRange) {
			LordLogInfo("HttpDownloader, support range, downloadByRange, m_contentLength:%u", m_contentLength.load());
			downloadByRange();
		}
		else {
			LordLogInfo("HttpDownloader, do not support range, downloadDirectly");
			downloadDirectly();
		}
	}

	m_cleanUpFinished = true;
}
