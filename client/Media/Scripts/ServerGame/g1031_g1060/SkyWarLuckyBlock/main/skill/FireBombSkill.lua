require "skill.BaseSkill"

FireBombSkill = class("FireBombSkill", BaseSkill)

function FireBombSkill:createSkillEffect(name, time)
    local player = self:getCreator()
    if player == nil then
        return
    end
    EngineWorld:getWorld():createExplosion(player:getEntityPlayer(), player:getEntityPlayer(),
            self.initPos, GameConfig.tntExplosionSize * 4, false)
    EngineWorld:addSimpleEffect("explosion.effect", self.initPos, player:getYaw(), 1000)
end

function FireBombSkill:onPlayerEffected(player)
    local effect = SkillEffectConfig:getSkillEffect(self.itemId)
    if effect == nil then
        return
    end
    if player:onMountHunt(tonumber(effect.value)) then
        return
    end

    local attacker = self:getCreator()
    local attackerUserId = 0
    if attacker ~= nil and attacker.userId ~= player.userId then
        attackerUserId = attacker.userId
    end
    player:updateHurtInfo(ReportUtil.DeadType.Skill, self.itemId, 0, attackerUserId)
    player:subHealth(tonumber(effect.value))
end

return FireBombSkill