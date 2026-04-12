---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 15:39
---

CriminalConfig = {}

CriminalConfig.initPos = {}
CriminalConfig.actor = {}
CriminalConfig.inventory = {}
CriminalConfig.data = {}
CriminalConfig.threatMultiple = 0
CriminalConfig.killPoliceThreat = 0

function CriminalConfig:init(config)
    self:initInitPos(config.initPos)
    self:initInventory(config.inventory)
    self:initActor(config.actor)
    self:initData(config.data)
    self:initThreatMultiple(config.threatMultiple)
    self:initKillPoliceThreat(config.killPoliceThreat)
end

function CriminalConfig:initInitPos(config)
    for i, pos in pairs(config) do
        self.initPos[i] = VectorUtil.newVector3(pos.x, pos.y, pos.z)
    end
end

function CriminalConfig:initInventory(config)
    for i, v in pairs(config) do
        local inventory = {}
        inventory.id = tonumber(v.id)
        inventory.num = tonumber(v.num)
        self.inventory[i] = inventory
    end
end

function CriminalConfig:initActor(config)
    for i, v in pairs(config) do
        local actor = {}
        actor.first = v.first
        actor.second = tonumber(v.second)
        self.actor[i] = actor
    end
end

function CriminalConfig:initData(config)
    for i, v in pairs(config) do
        local data = {}
        data.lv = tonumber(v.lv)
        if data.lv == 1 then
            data.role = GamePlayer.ROLE_PRISONER
        else
            data.role = GamePlayer.ROLE_CRIMINAL
        end
        data.name = tostring(v.name)
        data.nameKey = tostring(v.nameKey)
        data.threat = tonumber(v.threat)
        data.arrest = tonumber(v.arrest)
        data.kill = tonumber(v.kill)
        self.data[data.lv] = data
    end
    table.sort(self.data, function(a, b)
        return a.threat < b.threat
    end)
end

function CriminalConfig:initThreatMultiple(config)
    self.threatMultiple = config
end

function CriminalConfig:initKillPoliceThreat(config)
    self.killPoliceThreat = config
end

function CriminalConfig:getDataByThreat(threat)
    local criminal = self.data[1]
    for i, v in pairs(self.data) do
        if threat >= criminal.threat and threat < v.threat then
            break
        end
        criminal = v
    end
    return criminal
end

function CriminalConfig:getDataByLevel(level)
    local criminal = self.data[1]
    for i, v in pairs(self.data) do
        if v.lv == level then
            return v
        end
    end
    return criminal
end

function CriminalConfig:getThreatMultiple()
    return self.threatMultiple
end

function CriminalConfig:getInventory()
    return self.inventory
end

function CriminalConfig:getKillPoliceThreat()
    return self.killPoliceThreat
end


function CriminalConfig:randomInitPos()
    return self.initPos[math.random(1, #self.initPos)]
end

return CriminalConfig