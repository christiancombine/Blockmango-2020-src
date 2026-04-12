require "skill.BaseSkill"

FateDiceSkill = class("FateDiceSkill", BaseSkill)

local function getDicePosition(player)
    local yaw = player:getYaw()
    yaw = yaw % 360
    local dx, dz = 0, 0
    if yaw <= 22.5 or yaw > 337.5 then
        dz = 1
        dx = 0
    end
    if yaw > 22.5 and yaw <= 67.5 then
        dz = 1
        dx = -1
    end
    if yaw > 67.5 and yaw <= 112.5 then
        dz = 0
        dx = -1
    end
    if yaw > 112.5 and yaw <= 157.5 then
        dz = -1
        dx = -1
    end
    if yaw > 157.5 and yaw <= 202.5 then
        dz = -1
        dx = 0
    end
    if yaw > 202.5 and yaw <= 247.5 then
        dz = -1
        dx = 1
    end
    if yaw > 247.5 and yaw <= 292.5 then
        dz = 0
        dx = 1
    end
    if yaw > 292.5 and yaw <= 337.5 then
        dz = 1
        dx = 1
    end
    local pos = player:getPosition()
    return VectorUtil.newVector3(pos.x + dx, pos.y + 1, pos.z + dz)
end

function FateDiceSkill:onPlayerEffected(player)
    if player == nil then
        return
    end
    local effect = SkillEffectConfig:getSkillEffect(self.itemId)
    if effect == nil then
        return
    end
    local value = StringUtil.split(effect.value, "#")
    local pool = {}
    for index, item in pairs(value) do
        local pItem = {}
        pItem.id = index
        pItem.weight = value[index]
        table.insert(pool, pItem)
    end
    local result = RandomUtil:randomItemByWeight(1, pool, false)
    if result == nil or result[1] == nil then
        return
    end
    player:getFateDice(result[1].id)
    local pos = getDicePosition(player)
    local entityId = EngineWorld:addSessionNpc(pos, 0, -1, "g1054_touzi_donghua.actor", function(entity)
        entity:setActorBody("body")
        entity:setActorBodyId(tostring(result[1].id))
        entity:setCanCollided(false)
    end)
    LuaTimer:schedule(function(entityId)
        EngineWorld:removeEntity(entityId)
    end, 5000, nil, entityId)
end

return FateDiceSkill