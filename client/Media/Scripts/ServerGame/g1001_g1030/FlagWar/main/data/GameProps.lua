---
--- Created by Jimmy.
--- DateTime: 2018/6/11 0011 15:43
---
GameProps = class()

function GameProps:ctor(config)
    self.id = tonumber(config.id)
    self.life = tonumber(config.life)
    self.pos = VectorUtil.newVector3(config.pos[1], config.pos[2], config.pos[3])
    self.speeds = {}
    self:initSpeeds(config.speed)
end

function GameProps:initSpeeds(speeds)
    for i, speed in pairs(speeds) do
        local item = {}
        item.time = tonumber(speed.time)
        item.cd = tonumber(speed.cd)
        item.num = tonumber(speed.num)
        self.speeds[i] = item
    end
end

function GameProps:getSpeed(time)
    local result
    for i, speed in pairs(self.speeds) do
        if result == nil then
            result = speed
        end
        if time >= speed.time then
            result = speed
        end
    end
    return result
end

function GameProps:onTick(tick)
    local speed = self:getSpeed(tick)
    if speed == nil then
        return
    end
    if tick % speed.cd == 0 and speed.num > 0 then
        EngineWorld:addEntityItem(self.id, speed.num, 0, self.life, self.pos)
    end
end