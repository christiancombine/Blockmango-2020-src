require "skill.BaseSkill"

GoldenKnightSkill = class("GoldenKnightSkill", BaseSkill)

function GoldenKnightSkill:onPlayerEffected(player)
    local effect = SkillEffectConfig:getSkillEffect(self.itemId)
    if effect == nil then
        return
    end
    local param = StringUtil.split(effect.param, "#")
    local value = StringUtil.split(effect.value, "#")
    if player == nil then
        return
    end
    for index, item in pairs(param) do
        local pItem = ItemConfig:getItemById(item)
        if pItem ~= nil then
            if pItem.type == Define.ItemType.Armor then
                local canAdd = player:getInventory():isCanAddItem(pItem.itemId, pItem.meta, 1)
                player:equipArmor(pItem.itemId, pItem.meta)
                if not player:isEquipArmor(pItem.itemId, pItem.meta) and not canAdd then
                    local pos = player:getPosition()
                    EngineWorld:addEntityItem(pItem.itemId, 1, pItem.meta, 10000,
                            VectorUtil.newVector3(pos.x + 0.5, pos.y, pos.z + 0.5))
                end
            else
                player:addGameItem(pItem.itemId, tonumber(value[index]), pItem.meta)
            end
        end
    end
end

return GoldenKnightSkill