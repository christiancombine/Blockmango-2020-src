---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 15:38
---

PoliceConfig = {}

PoliceConfig.initPos = {}
PoliceConfig.confineRoomPos = {}
PoliceConfig.inventory = {}
PoliceConfig.actor = {}
PoliceConfig.data = {}
PoliceConfig.meritValue = 0
PoliceConfig.wageTime = 0
PoliceConfig.meritMultiple = 0
PoliceConfig.subMeritValue = 0
PoliceConfig.subMeritMultiple = 0


function PoliceConfig:init(config)
    self:initInitPos(config.initPos)
    self:initConfineRoomPos(config.confineRoomPos)
    self:initInventory(config.inventory)
    self:initActor(config.actor)
    self:initData(config.data)
    self:initMerit(config)
    self.wageTime = tonumber(config.wageTime)
end

function PoliceConfig:initInitPos(config)
    for i, pos in pairs(config) do
        self.initPos[i] = VectorUtil.newVector3(pos.x, pos.y, pos.z)
    end
end

function PoliceConfig:initConfineRoomPos(config)
    for i, pos in pairs(config) do
        self.confineRoomPos[i] = VectorUtil.newVector3(pos.x, pos.y, pos.z)
    end
end

function PoliceConfig:initInventory(config)
    for i, v in pairs(config) do
        local inventory = {}
        inventory.id = tonumber(v.id)
        inventory.num = tonumber(v.num)
        self.inventory[i] = inventory
    end
end

function PoliceConfig:getDataByLevel(level)
    local police = self.data[1]
    for i, v in pairs(self.data) do
        if v.lv == level then
            return v
        end
    end
    return police
end

function PoliceConfig:initActor(config)
    for i, v in pairs(config) do
        local actor = {}
        actor.first = v.first
        actor.second = tonumber(v.second)
        self.actor[i] = actor
    end
end

function PoliceConfig:initData(config)
    for i, v in pairs(config) do
        local data = {}
        data.role = GamePlayer.ROLE_POLICE
        data.lv = tonumber(v.lv)
        data.name = tostring(v.name)
        data.nameKey = tostring(v.nameKey)
        data.merit = tonumber(v.merit)
        data.wage = tonumber(v.wage)
        data.maxWage = tonumber(v.maxWage)
        self.data[i] = data
    end
    table.sort(self.data, function(a, b)
        return a.merit < b.merit
    end)
end


function PoliceConfig:initMerit(config)
    self.meritMultiple = config.meritMultiple
    self.meritValue = config.meritValue
    self.subMeritMultiple = config.subMeritMultiple
    self.subMeritValue = config.subMeritValue
end


function PoliceConfig:getDataByMerit(merit)
    local police = self.data[1]
    for i, v in pairs(self.data) do
        if merit >= police.merit and merit < v.merit then
            break
        end
        police = v
    end
    return police
end

function PoliceConfig:getMeritMultiple()
    return self.meritMultiple
end

function PoliceConfig:getMeritValue()
    return self.meritValue
end

function PoliceConfig:getSubMeritMultiple()
    return self.subMeritMultiple
end

function PoliceConfig:getSubMeritValue()
    return self.subMeritValue
end

function PoliceConfig:getInventory()
    return self.inventory
end

function PoliceConfig:randomInitPos()
    return self.initPos[math.random(1, #self.initPos)]
end

function PoliceConfig:randomConfineRoomPos()
    return self.confineRoomPos[math.random(1, #self.confineRoomPos)]
end


return PoliceConfig