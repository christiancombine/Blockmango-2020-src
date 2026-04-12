#include "ShellInterfaceAndroid.h"
#include "game.h"
#include "TypeDef.h"
#include "EchoHelper.h"

namespace GameClient {

    ShellInterfaceAndroid::ShellInterfaceAndroid() {

    }

    ShellInterfaceAndroid::~ShellInterfaceAndroid() {

    }

    bool ShellInterfaceAndroid::isLoginFromSdk() {
        return false;
    }

    void ShellInterfaceAndroid::login() {

    }

    void ShellInterfaceAndroid::logout() {

    }

//	void	ShellInterfaceAndroid::enterUserCenter()
//	{
//
//	}
//
//	void	ShellInterfaceAndroid::enterAppBBS()
//	{
//
//	}

    void ShellInterfaceAndroid::payWithInfo(const LORD::GUIString &payInfo) {
        onCallPayJNI(payInfo.c_str());
    }

    void ShellInterfaceAndroid::onUpdatePushInfo(const LORD::GUIString &pushInfo) {

    }

    void ShellInterfaceAndroid::onClearPushInfo() {

    }

    void ShellInterfaceAndroid::onCreateRoleSuccess(const LORD::GUIString &info) {

    }

    void ShellInterfaceAndroid::onLoginRoleSuccess(const LORD::GUIString &info) {

    }

    void ShellInterfaceAndroid::onRoleLevelUp(const LORD::GUIString &info) {

    }

    void ShellInterfaceAndroid::gameExit(LORD::String params) {
        gameExitJNI(params.c_str());
    }

    void ShellInterfaceAndroid::nextGame() {
        nextGameJNI();
    }

    void ShellInterfaceAndroid::onError(int errorCode) {
        onErrorJNI(errorCode);
    }


    void ShellInterfaceAndroid::usePropResults(String propId, int results) {
        usePropResultsJNI(propId.c_str(), results);
    }

    void ShellInterfaceAndroid::userIn(LORD::ui64 userId, LORD::String nickName) {
        userInJNI(userId, nickName.c_str());
    }

    void ShellInterfaceAndroid::userOut(LORD::ui64 userId) {
        userOutJNI(userId);
    }

    void ShellInterfaceAndroid::loadMapComplete() {
        loadMapCompleteJNI();
    }

    void ShellInterfaceAndroid::gameSettlement(LORD::String gameType, LORD::String gameResult) {
        gameSettlementJNI(gameType.c_str(), gameResult.c_str());
    }

    void ShellInterfaceAndroid::dataReport(LORD::String mainEvent, LORD::String childEvent) {
        dataReportJNI(mainEvent.c_str(), childEvent.c_str());
    }

    void ShellInterfaceAndroid::dataReportError(LORD::String errorText) {
        dataReportErrorJNI(errorText.c_str());
    }

    void ShellInterfaceAndroid::userChange(LORD::String userName, LORD::String teamName, LORD::ui64 userId, LORD::ui32 teamId, bool isUserIn) {
        userChangeJNI(userName.c_str(), teamName.c_str(), userId, teamId, isUserIn);
    }

    void ShellInterfaceAndroid::onFriendOperation(int operationType, LORD::ui64 userId) {
        onFriendOperationJNI(operationType, userId);
    }

    void ShellInterfaceAndroid::resetGame(LORD::ui64 targetUserId, LORD::String gameType, LORD::String mapId) {
        resetGameJNI(targetUserId, gameType.c_str(), mapId.c_str());
    }

    void ShellInterfaceAndroid::resetGameAddr(LORD::ui64 targetUserId, LORD::String gameType, LORD::String mapId, LORD::String addr) {
        resetGameAddrJNI(targetUserId, gameType.c_str(), mapId.c_str(), addr.c_str());
    }

    void ShellInterfaceAndroid::closeGame(int code) {
        closeGameJNI(code);
    }

    void ShellInterfaceAndroid::onRecharge(int type, LORD::String params) {
        onRechargeJNI(type, params.c_str());
    }
	
	void ShellInterfaceAndroid::buyAction(int type) {
        buyActionJNI(type);
    }

    void ShellInterfaceAndroid::playVideo(int type, LORD::String path) {
        playVideoJNI(type, path.c_str());
    }

    void ShellInterfaceAndroid::playAdVideo() {
        playAdVideoJNI();
    }

    void ShellInterfaceAndroid::reportAdVideoShow() {
        reportAdVideoShowJNI();
    }

    void ShellInterfaceAndroid::playScreen(int type) {
        playScreenJNI(type);
    }

    bool ShellInterfaceAndroid::getBoolForKey(LORD::String key, bool defaultValue)
    {
        return getBoolForKeyJNI(key.c_str(), defaultValue);
    }

    void ShellInterfaceAndroid::putBoolForKey(LORD::String key, bool value)
    {
        setBoolForKeyJNI(key.c_str(), value);
    }

	LORD::String ShellInterfaceAndroid::getStringForKey(LORD::String key, LORD::String defaultValue)
	{
		return getStringForKeyJNI(key.c_str(), defaultValue.c_str()).c_str();
	}

	void ShellInterfaceAndroid::putStringForKey(LORD::String key, LORD::String value)
	{
	    setStringForKeyJNI(key.c_str(), value.c_str());
	}

    void ShellInterfaceAndroid::onExpResult(int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status) {
        onExpResultJNI(curLv, toLv, addExp, curExp, toExp, upExp, status);
    }

    LORD::String ShellInterfaceAndroid::getClientInfo() {
        return getClientInfoJNI().c_str();
    }

    void ShellInterfaceAndroid::onWatchAd(int type, LORD::String params, LORD::String adsId) {
        onWatchAdJNI(type, params.c_str(), adsId.c_str());
    }

    void ShellInterfaceAndroid::onAppActionTrigger(int type, LORD::String params) {
        onAppActionTriggerJNI(type, params.c_str());
    }

    void ShellInterfaceAndroid::onMapDownloadSuccess() {
        onMapDownloadSuccessJNI();
    }

    int ShellInterfaceAndroid::getGameTimeToday() {
        return getGameTimeTodayJNI();
    }

    void ShellInterfaceAndroid::followEnterGame(LORD::ui64 targetId, const LORD::String& gameType) {
        followEnterGameJNI(targetId, gameType.c_str());
    }

}


