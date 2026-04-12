#pragma once
#ifndef __BASE_VALUE_PROXY_HEADER_H__
#define __BASE_VALUE_PROXY_HEADER_H__

#include "LuaRegister/Template/LuaRegister.h"
#include "Script/VarProxy/BaseValueProxy.h"

_BEGIN_REGISTER_CLASS(FloatProxy)
_CLASSREGISTER_AddVar(value, FloatProxy::value)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(BoolProxy)
_CLASSREGISTER_AddVar(value, BoolProxy::value)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(IntProxy)
_CLASSREGISTER_AddVar(value, IntProxy::value)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(LongProxy)
_CLASSREGISTER_AddVar(value, LongProxy::value)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(UI64Proxy)
_CLASSREGISTER_AddVar(value, UI64Proxy::value)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(StringProxy)
_CLASSREGISTER_AddVar(value, StringProxy::value)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(Vector3iProxy)
_CLASSREGISTER_AddVar(x, Vector3iProxy::x)
_CLASSREGISTER_AddVar(y, Vector3iProxy::y)
_CLASSREGISTER_AddVar(z, Vector3iProxy::z)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(Vector3Proxy)
_CLASSREGISTER_AddVar(x, Vector3Proxy::x)
_CLASSREGISTER_AddVar(y, Vector3Proxy::y)
_CLASSREGISTER_AddVar(z, Vector3Proxy::z)
_END_REGISTER_CLASS()
#endif