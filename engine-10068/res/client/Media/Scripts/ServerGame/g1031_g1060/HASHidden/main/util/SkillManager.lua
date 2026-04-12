---
--- Created by xiaolai.
--- DateTime: 2018/11/13 0013 12:00
---
SkillManager = {}

function SkillManager:useColorfulGenades(initpos, area, player, effect, duration)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role ~= player.role then
            local position = v:getPosition()
            if area:inArea(position) then
                v.entityPlayerMP:setOnColorful(duration * 1000)
            end
        end
    end
end

function SkillManager:useSpeedUp(player, duration)
    player:addEffect(PotionID.MOVE_SPEED, duration, 1)
end

return SkillManager