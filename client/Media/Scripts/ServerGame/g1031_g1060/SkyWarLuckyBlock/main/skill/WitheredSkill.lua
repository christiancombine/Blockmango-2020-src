require "skill.BaseSkill"

WitheredSkill = class("WitheredSkill", BaseSkill)

function WitheredSkill:createSkillEffect(name, time)

end

function WitheredSkill:onPlayerEffected(player)
    if player:ifOnMount() then
        return
    end
    local config = SkillEffectConfig:getSkillEffect(self.itemId)
    if not config then
        return
    end
    local params = StringUtil.split(config.param, "#")
    local id = tonumber(params[1])
    local itemId = tonumber(params[2])
    local hp = tonumber(config.value)
    local position = VectorUtil.add3(player:getPosition(), VectorUtil.newVector3(0, 2, 0))
    GamePacketSender:sendSetLuckySkyView(player.rakssid)
    GameWithered.new(id, position, hp, self.duration, itemId, player)
end

return WitheredSkill