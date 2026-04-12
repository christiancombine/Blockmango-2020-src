require "data.GameSkill"
LogicListener = {}

function LogicListener:init()
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
    ItemSkillAttackEvent.registerCallBack(self.onItemSkillAttack)
end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    return false
end

function LogicListener.onItemSkillAttack(entityId, itemId, position)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return
    end
    if player.isAnimal == true then
        return
    end
    if player.isLife == false or GameMatch:isGameStart() == false then
        return
    end
    local skill = SkillConfig:getSkillById(itemId)
    if skill ~= nil then
        GameSkill.new(skill, player, position)
    end
    if skill.useTime == 1 then
        player:removeItem(itemId, 1)
    end
end
return LogicListener