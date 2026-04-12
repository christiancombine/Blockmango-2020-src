---
--- Created by Jimmy.
--- DateTime: 2017/11/14 0014 16:10
---
require "data.GameMerchant"
require "data.GameTransferGate"
require "data.GameBloodPool"
require "data.GameProps"
require "data.GameFlagStation"
require "data.GameFlag"
require "config.Area"

Scene = class()
Scene.teams = {}
Scene.merchants = {}
Scene.transferGates = {}
Scene.bloodPools = {}
Scene.props = {}
Scene.flagStations = {}
Scene.flags = {}

function Scene:ctor(config)
    self:init(config)
end

function Scene:init(config)
    self.name = tonumber(config.name)
    self:initTeams(config.teams)
    self:initMerchants(config.merchants)
    self:initTransferGates(config.transferGates)
    self:initBloodPools(config.bloodPools)
    self:initProps(config.props)
    self:initFlagStations(config.flagStations)
    self:initFlags(config.flags)
end

function Scene:initTeams(teams)
    for i, v in pairs(teams) do
        self.teams[v.id] = {}
        self.teams[v.id].id = v.id
        self.teams[v.id].name = v.name
        self.teams[v.id].initPos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        self.teams[v.id].area = Area.new(v.area)
    end
end

function Scene:initMerchants(merchants)
    for i, v in pairs(merchants) do
        local merchant = {}
        merchant.teamId = tonumber(v.teamId)
        merchant.name = v.name
        merchant.initPos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        merchant.yaw = tonumber(v.initPos[4])
        merchant.goods = {}
        for j, g in pairs(v.goods) do
            local item = {}
            item.lv = tonumber(g.lv)
            item.time = tonumber(g.time)
            item.groupId = tonumber(g.groupId)
            merchant.goods[item.lv] = item
        end
        self.merchants[merchant.teamId] = merchant
    end
end

function Scene:initTransferGates(transferGates)
    for i, transferGate in pairs(transferGates) do
        self.transferGates[i] = GameTransferGate.new(transferGate)
    end
end

function Scene:initBloodPools(bloodPools)
    for i, bloodPool in pairs(bloodPools) do
        self.bloodPools[i] = GameBloodPool.new(bloodPool)
    end
end

function Scene:initProps(props)
    for i, prop in pairs(props) do
        self.props[i] = GameProps.new(prop)
    end
end

function Scene:initFlagStations(flagStations)
    for i, flagStation in pairs(flagStations) do
        self.flagStations[i] = GameFlagStation.new(flagStation)
    end
end

function Scene:initFlags(flags)
    for i, flag in pairs(flags) do
        self.flags[i] = GameFlag.new(flag)
    end
end

function Scene:getTeams()
    return self.teams
end

function Scene:prepareMerchants()
    for teamId, merchant in pairs(self.merchants) do
        merchant.npc = GameMerchant.new(merchant)
    end
end

function Scene:getTransferGates()
    return self.transferGates
end

function Scene:getBloodPools()
    return self.bloodPools
end

function Scene:getFlagStations()
    return self.flagStations
end

function Scene:onTick(tick)
    for teamId, merchant in pairs(self.merchants) do
        if merchant.npc then
            merchant.npc:onTick(tick)
        end
    end
    for index, transferGate in pairs(self.transferGates) do
        transferGate:onTick(tick)
    end
    for index, bloodPool in pairs(self.bloodPools) do
        bloodPool:onTick(tick)
    end
    for index, prop in pairs(self.props) do
        prop:onTick(tick)
    end
    for index, flag in pairs(self.flags) do
        flag:onTick(tick)
    end
end

function Scene:onPlayerQuit(player)
    for index, flag in pairs(self.flags) do
        flag:onPlayerQuit(player)
    end
end

function Scene:onPlayerDie(player)
    for index, flag in pairs(self.flags) do
        flag:onPlayerDie(player)
    end
end

function Scene:getFlagByEntityId(entityId)
    for index, flag in pairs(self.flags) do
        if flag.entityId == entityId then
            return flag
        end
    end
end

function Scene:reset()
    for teamId, merchant in pairs(self.merchants) do
        if merchant.npc then
            merchant.npc:onRemove()
            merchant.npc = nil
        end
    end
    for index, transferGate in pairs(self.transferGates) do
        transferGate:clearRecord()
    end
    for index, bloodPool in pairs(self.bloodPools) do
        bloodPool:clearRecord()
    end
    for index, flagStation in pairs(self.flagStations) do
        flagStation:clearRecord()
    end
    for index, flag in pairs(self.flags) do
        flag:clearRecord()
    end
end

return Scene