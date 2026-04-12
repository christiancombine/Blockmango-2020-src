#pragma once

#ifndef _HOST_API_REGISTER_H__
#define _HOST_API_REGISTER_H__

#include "Plugins/HostApi.h"
#include "LuaRegister/Template/LuaRegister.h"

using namespace LUA_REGISTER;

_BEGIN_REGISTER_CLASS(HostApi)
_CLASSREGISTER_AddStaticMember(log, HostApi::log)
_CLASSREGISTER_AddStaticMember(sendMsg, HostApi::sendMsg)
_CLASSREGISTER_AddStaticMember(sendMsgByType, HostApi::sendMsgByType)
_CLASSREGISTER_AddStaticMember(dataReport, HostApi::dataReport)
_CLASSREGISTER_AddStaticMember(onRecharge, HostApi::onRecharge)
_CLASSREGISTER_AddStaticMember(putBoolForKey, HostApi::putBoolForKey)
_CLASSREGISTER_AddStaticMember(getBoolForKey, HostApi::getBoolForKey)
_CLASSREGISTER_AddStaticMember(putStringForKey, HostApi::putStringForKey)
_CLASSREGISTER_AddStaticMember(getStringForKey, HostApi::getStringForKey)
_CLASSREGISTER_AddStaticMember(onWatchAd, HostApi::onWatchAd)
_CLASSREGISTER_AddStaticMember(onAppActionTrigger, HostApi::onAppActionTrigger)
_CLASSREGISTER_AddStaticMember(getClientInfo, HostApi::getClientInfo)
_CLASSREGISTER_AddStaticMember(putBoolPrefs, HostApi::putBoolPrefs)
_CLASSREGISTER_AddStaticMember(getBoolPrefs, HostApi::getBoolPrefs)
_CLASSREGISTER_AddStaticMember(putIntPrefs, HostApi::putIntPrefs)
_CLASSREGISTER_AddStaticMember(getIntPrefs, HostApi::getIntPrefs)
_CLASSREGISTER_AddStaticMember(putFloatPrefs, HostApi::putFloatPrefs)
_CLASSREGISTER_AddStaticMember(getFloatPrefs, HostApi::getFloatPrefs)
_CLASSREGISTER_AddStaticMember(putStringPrefs, HostApi::putStringPrefs)
_CLASSREGISTER_AddStaticMember(getStringPrefs, HostApi::getStringPrefs)
_CLASSREGISTER_AddStaticMember(showNotification, HostApi::showNotification)
_CLASSREGISTER_AddStaticMember(setClientAction, HostApi::setClientAction)
_CLASSREGISTER_AddStaticMember(addFunctionConfig, HostApi::addFunctionConfig)
_CLASSREGISTER_AddStaticMember(reportAdVideoShow, HostApi::reportAdVideoShow)
_CLASSREGISTER_AddStaticMember(notifyGetMoney, HostApi::notifyGetMoney)
_CLASSREGISTER_AddStaticMember(notifyGetItem, HostApi::notifyGetItem)
_CLASSREGISTER_AddStaticMember(notifyGetGoods, HostApi::notifyGetGoods)
_CLASSREGISTER_AddStaticMember(followEnterGame, HostApi::followEnterGame)
_END_REGISTER_CLASS()

#endif