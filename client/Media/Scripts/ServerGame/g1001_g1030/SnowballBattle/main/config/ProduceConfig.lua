---
--- Created by Lester.
--- DateTime: 2017/11/20 0020 11:28
---

ProduceConfig = class()

function ProduceConfig:ctor(config)
    self.produce = {}
    self.records = {}
    self:init(config)
end

function ProduceConfig:init(config)
    if config == nil then
        HostApi.log("ProduceConfig is nil")
        return
    end
    for i, v in pairs(config) do
        local cf = {}

        local pos = {}
        pos.x = tonumber(v.pos[1])
        pos.y = tonumber(v.pos[2])
        pos.z = tonumber(v.pos[3])
        local signPos = {}
        signPos.x = tonumber(v.signPos[1])
        signPos.y = tonumber(v.signPos[2])
        signPos.z = tonumber(v.signPos[3])
        cf.pos = pos
        cf.signPos = signPos

        cf.name = v.name
        cf.id = tonumber(v.id)
        cf.life = tonumber(v.life)
        cf.interval = tonumber(v.interval)
        cf.enableCD = v.enableCD

        cf.lv = {}
        for k, v in pairs(v.lv) do
            local lv = {}
            lv.num = tonumber(v.num)
            lv.upgradeTime = tonumber(v.upgradeTime)
            cf.lv[k] = lv
        end

        self.produce[i] = cf
    end
end

function ProduceConfig:getAllProduce()
    return self.produce
end

function ProduceConfig:addPropToWorld(ticks)
    for i, prop in pairs(self.produce) do
        if prop == nil then
            break
        end
        local record = self.records[VectorUtil.hashVector3(prop.pos)]
        if record == nil or ticks >= record.upgradeTime + record.lastTime then
            if record == nil then
                record = {}
                record.lv = 0
                record.num = 0
                record.upgradeTime = 0
            end
            record.lastTime = ticks

            local lvConfig = prop.lv[record.lv + 1]
            if lvConfig ~= nil then
                record.lv = record.lv + 1
                record.num = lvConfig.num
                record.upgradeTime = lvConfig.upgradeTime
                self.records[VectorUtil.hashVector3(prop.pos)] = record
            end

        end

        local cd = prop.interval - ticks % (prop.interval + 1)

        if prop.enableCD then
            local signPos = VectorUtil.newVector3i(prop.signPos.x, prop.signPos.y, prop.signPos.z)
            local second = cd % 60
            local minute = math.floor(cd / 60)
            EngineWorld:getWorld():resetSignText(signPos, 1, tostring(minute) .. ":" .. tostring(second))
        end
        if cd == 0 and record.num > 0 then
            local producePos = VectorUtil.newVector3(prop.pos.x, prop.pos.y + 1, prop.pos.z)
            EngineWorld:addEntityItem(prop.id, record.num, 0, prop.life, producePos)
        end
    end
end

return ProduceConfig