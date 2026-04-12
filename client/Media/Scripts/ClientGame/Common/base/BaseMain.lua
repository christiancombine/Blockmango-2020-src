---
--- Created by Jimmy.
--- DateTime: 2018/10/9 0009 15:02
---
local paths = {}
string.gsub(package.path, '[^;]+', function(w)
    table.insert(paths, w)
end)
local path = paths[#paths]
package.path = package.path .. ";" .. path:gsub("ClientGame", "BaseGame")

isClient = true
isTest = EngineVersionSetting.getEngineVersion() >= 90000

require "base.Base"
require "base.define.GUIDefine"
require "base.util.ClientHelper"
require "base.util.DynamicCast"
require "base.util.MemoryPool"
require "base.util.MsgSender"
require "base.util.PayHelper"
require "base.util.SoundUtil"
require "base.util.FileUtil"
require "base.util.Lang"
require "base.util.CommonUtil"
SLAXML = require 'base.xml.slaxml'
SLAXDOM = require 'base.xml.slaxdom'
require "base.packet.PacketSender"
require "base.packet.DataBuilder"
require "base.entity.IEntity"
require "base.entity.EntityCache"
require "base.event.EventObservable"
require "base.event.CommonDataEvents"
require "base.event.IScriptEvent"
require "base.event.GameEvents"
require "base.analytics.GameAnalytics"
require "base.analytics.GameAnalyticsCache"
require "base.web.WebService"
require "base.listener.Listener"
require "base.listener.AnalyticsListener"
require "base.listener.BaseListener"
require "base.data.EngineWorld"
require "base.data.IArea"
require "base.data.PlayerWallet"
require "base.data.BasePlayer"
require "base.data.BaseProperty"
require "base.ui.GUIManager"
require "base.ui.IGUIWindow"
require "base.ui.IGUILayout"
require "base.ui.IGUIDataView"
require "base.ui.IGUIGridView"
require "base.ui.IGUIListView"
require "base.ui.anim.GUISubAnim"
require "base.ui.anim.GUISubAnimScale"
require "base.ui.anim.GUISubAnimAlpha"
require "base.ui.anim.GUISubAnimMove"
require "base.ui.anim.GUIAnim"
require "base.ui.anim.GUIAnimManager"
require "base.ui.adapter.IDataAdapter"
require "base.task.LuaTimer"
require "base.Game"
require "base.datastructure.queue"
require "base.manager.PlayerManager"
require "base.manager.TaskQueueManager"
require "base.config.FunctionSetting"
require "base.helper.MaxRecordHelper"
require "base.helper.ExchangeHelper"
require "base.messages.TextFormat"

function print_lua_table(lua_table, indent)
    indent = indent or 0
    for k, v in pairs(lua_table) do
        if type(k) == "string" then
            k = string.format("%q", k)
        end
        local szSuffix = ""
        if type(v) == "table" then
            szSuffix = "{"
        end
        local szPrefix = string.rep("    ", indent)
        formatting = szPrefix .. "[" .. k .. "]" .. " = " .. szSuffix
        if type(v) == "table" then
            HostApi.log(formatting)
            print_lua_table(v, indent + 1)
            HostApi.log(szPrefix .. "},")
        else
            local szValue = ""
            if type(v) == "string" then
                szValue = string.format("%q", v)
            else
                szValue = tostring(v)
            end
            HostApi.log(formatting .. szValue .. ",")
        end
    end
end

local oldG = {}
local oldLoaded = {}

---保存当前的全局环境
local function saveCurrentG()
    for key, value in pairs(_G) do
        oldG[key] = value
    end
    local loaded = _G["package"]["loaded"]
    for key, value in pairs(loaded) do
        oldLoaded[key] = value
    end
end

---恢复成原来的全局环境
local function restoreOldG()
    for key, value in pairs(_G) do
        if oldG[key] == nil then
            _G[key] = nil
        end
    end
    local loaded = _G["package"]["loaded"]
    for key, value in pairs(loaded) do
        if oldLoaded[key] == nil then
            loaded[key] = nil
        end
    end
end

BaseListener:init()
TaskQueueManager:init()
FunctionSetting:init()

BaseMain = {}
BaseMain.GameType = "g1001"

function BaseMain:setGameType(GameType)
    self.GameType = GameType
end

function BaseMain:getGameType()
    return self.GameType
end

local DataProcessor = require "base.packet.processor.DataBuilderProcessor"
DataBuilderProcessor = DataProcessor.new()
local JsonProcessor = require "base.packet.processor.JsonBuilderProcessor"
JsonBuilderProcessor = JsonProcessor.new()

saveCurrentG()

function BaseMain:destroy()
    restoreOldG()
end

CommonDataEvents:registerCallBack("ServerInfo", function(data)
    local isChina = data:getBoolParam("isChina")
    Root.Instance():setChina(isChina)
end, DataBuilderProcessor)

return BaseMain