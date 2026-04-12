
GameChampion = class("GameActor")

function GameChampion:ctor(config)
    self.id = tonumber(config.id) or 1
    self.type = tonumber(config.type) or 0
    self.actor = "g1055_blackman.actor"
    self.pos = self:initPosFromConfig(config.pos)
    self.yaw = tonumber(config.yaw) or 0
    self.scale = tonumber(config.scale) or 1
    self.height = tonumber(config.height) or 1
    self.textScale = tonumber(config.textScale) or 1.0
    self.topInfo = tostring(config.topInfo) or ""
    self.lv = tonumber(config.lv) or 1
    self:onCreate()
end

function GameChampion:onCreate()
    local entityId = EngineWorld:addActorNpc(self.pos, self.yaw, self.actor, function(entity)
        entity:setWeight("-1")
        entity:setHeadName(string.format("%s=%s", self.topInfo, "name"))
        -- LogUtil.log("GameChampion " ..self.scale .. " " .. self.textScale)
        entity:setScale(self.scale)
        entity:setFloatProperty("textScale", self.textScale)

        local entityActorNpc = DynamicCastEntity.dynamicCastActorNpc(entity)
        if entityActorNpc then
            entityActorNpc:setHeight(self.height)
        end
    end)
    self.entityId = entityId
end

function GameChampion:initPosFromConfig(configPos)
    local vec3 = StringUtil.split(configPos, "#")
    return VectorUtil.newVector3(tonumber(vec3[1] or 0), tonumber(vec3[2] or 0), tonumber(vec3[3]) or 0)
end

return GameChampion