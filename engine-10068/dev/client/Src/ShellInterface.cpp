#include "ShellInterface.h"
#include "TypeDef.h"
#include "game.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Setting/AdvertSetting.h"
#include "Setting/FunctionSetting.h"
#include "Util/PlayerPrefs.h"
#ifdef WIN32
#include "shellapi.h"
#endif

namespace GameClient
{
	ShellInterface::ShellInterface()
	{

	}

	ShellInterface::~ShellInterface()
	{

	}

	void ShellInterface::login()
	{

	}

	void ShellInterface::logout()
	{

	}

	void ShellInterface::enterUserCenter()
	{

	}

	void ShellInterface::enterAppBBS()
	{

	}

	void ShellInterface::payWithInfo(const LORD::GUIString& payInfo)
	{
		//LORD::String gbkUrl = StringUtil::UTF8ToGBK();
#ifdef WIN32
		ShellExecute(0, "open", "iexplore.exe", payInfo.c_str(), NULL, SW_HIDE);
#endif
	}

	bool ShellInterface::islogin()
	{
		return true;
	}

	bool ShellInterface::isLoginFromSdk()
	{
		return false;
	}

	void ShellInterface::onUpdatePushInfo(const LORD::GUIString& pushInfo)
	{

	}

	void ShellInterface::onClearPushInfo()
	{

	}

	LORD::GUIString ShellInterface::getDeviceInfo()
	{
		LORD::String phonetype = CGame::Instance()->GetPhoneInfo();
		LORD::String gouinfo = CGame::Instance()->GetGPUInfo();
		LORD::GUIString ret((phonetype + "|" + gouinfo).c_str());
		return ret;
	}

	void ShellInterface::gameExit(LORD::String params)
	{

	}

	void ShellInterface::nextGame()
	{
	}

	void ShellInterface::closeGame(int code)
	{
	}

	void ShellInterface::onError(int errorCode)
	{
	}

	void ShellInterface::usePropResults(LORD::String propId, int results)
	{
	}

	void ShellInterface::userIn(LORD::ui64 userId, LORD::String nickName)
	{
	}

	void ShellInterface::userOut(LORD::ui64 userId)
	{
	}

	void ShellInterface::loadMapComplete()
	{
	}

	void ShellInterface::gameSettlement(LORD::String gameType, LORD::String gameResult)
	{
	}

	void ShellInterface::dataReport(LORD::String mainEvent, LORD::String childEvent)
	{
	}

	void ShellInterface::userChange(LORD::String userName, LORD::String teamName, LORD::ui64 userId, LORD::ui32 teamId, bool isUserIn)
	{
	}

	void ShellInterface::onFriendOperation(int operationType, LORD::ui64 userId)
	{
	}

	void ShellInterface::resetGame(LORD::ui64 targetUserId, LORD::String gameType, LORD::String mapId)
	{
	}

	void ShellInterface::resetGameAddr(LORD::ui64 targetUserId, LORD::String gameType, LORD::String mapId, LORD::String addr)
	{
	}

	void ShellInterface::onRecharge(int type)
	{
		ui64 userId = GameClient::CGame::Instance()->getPlatformUserId();
		int enableSumRecharge = PlayerPrefs::Instance()->getIntPrefs("EnableSumRecharge");
		int enableTimeRecharge = PlayerPrefs::Instance()->getIntPrefs("EnableTimeRecharge");
		String gameType = PlayerPrefs::Instance()->HasProperty("RechargeGameType") ?
			PlayerPrefs::Instance()->getStringPrefs("RechargeGameType") : GameClient::CGame::Instance()->getGameType();
		String params = "userId=" + StringUtil::ToString(userId);
		params += "&enableSumRecharge=" + StringUtil::ToString(enableSumRecharge);
		params += "&enableTimeRecharge=" + StringUtil::ToString(enableTimeRecharge);
		params += "&gameType=" + gameType;
		LordLogInfo("[onRecharge] [type]=[%d] [params]=[%s]", type, params.c_str());
		if (FunctionSetting::Instance()->enabledRecharge())
		{
			onRecharge(type, params);
		}
	}
	
	void ShellInterface::buyAction(int type)
	{
		
	}

	void ShellInterface::playVideo(int type, LORD::String path)
	{
	}

	bool ShellInterface::getBoolForKey(LORD::String key, bool defaultValue)
	{
		if (PlayerPrefs::Instance()->HasProperty(key.c_str()))
		{
			return PlayerPrefs::Instance()->getBoolPrefs(key);
		}
		return defaultValue;
	}

	void ShellInterface::putBoolForKey(LORD::String key, bool value)
	{
		PlayerPrefs::Instance()->putBoolPrefs(key, value);
	}

	LORD::String ShellInterface::getStringForKey(LORD::String key, LORD::String defaultValue)
	{
		if (PlayerPrefs::Instance()->HasProperty(key.c_str()))
		{
			return PlayerPrefs::Instance()->getStringPrefs(key);
		}
		return defaultValue;
	}

	void ShellInterface::putStringForKey(LORD::String key, LORD::String value)
	{
		PlayerPrefs::Instance()->putStringPrefs(key, value);
	}

	LORD::String ShellInterface::getClientInfo()
	{
		auto doc = rapidjson::Document();
		doc.SetObject();
		rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
		rapidjson::Value data(rapidjson::kObjectType);
		data.AddMember("device", "Windows", allocator);
		data.AddMember("user_id", "8888888", allocator);
		data.AddMember("os_version", "windows 7", allocator);
		data.AddMember("manufacturer", "lenovo", allocator);
		data.AddMember("platform", "windows", allocator);
		data.AddMember("session_num", 1, allocator);
		data.AddMember("version_code", 1, allocator);
		data.AddMember("package_name", "com.sandboxol.blockymods", allocator);
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		data.Accept(writer);
		return String(buffer.GetString());
	}

	void ShellInterface::onWatchAd(int type, LORD::String params)
	{
		String AdsId = AdvertSetting::Instance()->getAdsId();
		onWatchAd(type, params, AdsId);
	}

	void ShellInterface::playAdVideo()
	{

	}

	void ShellInterface::reportAdVideoShow()
	{

	}

	void ShellInterface::playScreen(int type)
	{
	}

}

