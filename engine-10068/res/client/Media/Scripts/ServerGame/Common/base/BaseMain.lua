---
--- Created by Jimmy.
--- DateTime: 2018/10/9 0009 15:02
---

if LogicSetting.Instance():getLordPlatform() == 1 then
    local paths = {}
    string.gsub(package.path, '[^;]+', function(w)
        table.insert(paths, w)
    end)
    local path = paths[#paths]
    package.path = package.path .. ";" .. path:gsub("ServerGame", "BaseGame")
end

isClient = false
isTest = EngineVersionSetting.getEngineVersion() >= 90000
GameType = "g1001"

require "base.Base"
require "base.define.ScoreID"
require "base.util.DynamicCast"
require "base.util.EngineUtil"
require "base.util.InventoryUtil"
require "base.util.MerchantUtil"
require "base.util.MsgSender"
require "base.util.WalletUtils"
require "base.util.FileUtil"
require "base.util.BaseUtil"
require "base.util.PointUtil"
require "base.util.RectUtil"
require "base.util.ServerHelper"
require "base.util.RespawnHelper"
require "base.config.SumRechargeConfig"
require "base.recharge.SumRechargeHelper"
require "base.event.CommonDataEvents"
require "base.event.EventObservable"
require "base.event.IScriptEvent"
require "base.event.GameEvents"
require "base.analytics.GameAnalytics"
require "base.analytics.GameAnalyticsCache"
require "base.cache.PlayerIdentityCache"
require "base.cache.UserInfoCache"
require "base.code.GameOverCode"
require "base.code.MailType"
require "base.data.BaseBtTree"
require "base.entity.IEntity"
require "base.entity.EntityCache"
require "base.data.IArea"
require "base.data.BasePlayer"
require "base.data.EngineWorld"
require "base.exp.ExpRule"
require "base.exp.UserExpManager"
require "base.listener.AnalyticsListener"
require "base.listener.BaseListener"
require "base.messages.IMessages"
require "base.messages.TextFormat"
require "base.packet.PacketSender"
require "base.packet.DataBuilder"
require "base.pay.PayHelper"
require "base.pay.OrderManager"
require "base.prop.AppProps"
require "base.rank.RankManager"
require "base.season.SeasonManager"
require "base.task.ITask"
require "base.task.CommonTask"
require "base.task.LuaTimer"
require "base.web.WebRequestType"
require "base.web.WebService"
require "base.DBManager"
require "base.GameGroupManager"
require "base.GameServer"
require "base.PlayerManager"
require "base.ReportManager"
require "base.RewardManager"
require "base.helper.MaxRecordBuilder"
require "base.helper.MaxRecordHelper"
require "base.helper.ExchangeHelper"

AnalyticsListener:init()
BaseListener:init()

BaseMain = {}
BaseMain.GameType = "g1001"
BaseMain.IsChina = false

function BaseMain:setGameType(GameType)
    self.GameType = GameType
    DBManager:setGameType(self.GameType)
    ReportManager:setGameType(self.GameType)
    RewardManager:setGameType(self.GameType)
    HostApi.setServerGameType(GameType)
end

function BaseMain:getGameType()
    return self.GameType
end

function BaseMain:setIsChina(isChina)
    self.IsChina = isChina
end

function BaseMain:isChina()
    return self.IsChina
end

local DataProcessor = require "base.packet.processor.DataBuilderProcessor"
DataBuilderProcessor = DataProcessor.new()
local JsonProcessor = require "base.packet.processor.JsonDataProcessor"
JsonBuilderProcessor = JsonProcessor.new()

-----表示当收集器在总使用内存数量达到上次垃圾收集时的1.2倍(默认2倍)时再开启新的收集周期
collectgarbage("setpause", 120)
-----表示垃圾收集器的运行速度是内存分配的5(默认2倍)倍
collectgarbage("setstepmul", 500)