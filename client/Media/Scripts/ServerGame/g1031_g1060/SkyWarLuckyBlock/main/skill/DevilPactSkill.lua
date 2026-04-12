require "skill.BaseSkill"

DevilPactSkill = class("DevilPactSkill", BaseSkill)

function DevilPactSkill:onPlayerEffected(player)
    local effect = SkillEffectConfig:getSkillEffect(self.itemId)
    if effect == nil then
        return
    end
    local param = StringUtil.split(effect.param, "#")
    local value = StringUtil.split(effect.value, "#")
    if player == nil then
        return
    end
    player:removeCurItem()
    player:onDropItem(self.itemId)
    for index, item in pairs(param) do
        local pItem = ItemConfig:getItemById(item)
        if pItem ~= nil then
            if pItem.type == Define.ItemType.Armor then
                player:equipArmor(pItem.itemId, pItem.meta)
            else
                player:addGameItem(pItem.itemId, tonumber(value[index]), pItem.meta)
            end
        end
    end
    player.signDevilPack = false

    if player.inDevilPact == true then
        return
    end

    GamePacketSender:sendLuckySkyRopeDate(player.rakssid, self.duration)
    player.inDevilPact = true
end

function DevilPactSkill:onDestroy()
    local player = self:getCreator()
    if player == nil or GameMatch:isGameOver() or player.isLife == false then
        return
    end

    local attackerUserId = 0
    if os.time() - player.hurtInfo.time > 10 then
        attackerUserId = player.userId
    end
    player:updateHurtInfo(ReportUtil.DeadType.Skill, self.itemId, 0, attackerUserId)

    player.killByDevil = true
    LuaTimer:schedule(function(userId)
        local c_player = PlayerManager:getPlayerByUserId(userId)
        if c_player == nil or GameMatch:isGameOver() then
            return
        end
        c_player.killByDevil = false
        if c_player.isLife ~= false then
            c_player:subHealth(99999)
        end
    end, 500, nil, player.userId)
    player:subHealth(99999)
end

return DevilPactSkill