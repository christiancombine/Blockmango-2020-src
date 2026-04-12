---
--- Created by Jimmy.
--- DateTime: 2018/3/9 0009 11:29
---

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
    if GameMatch:isGameRunning() == false then
        MsgSender.sendTopTipsToTarget(player.rakssid, 3, Messages:msgUseSkillNoRunning())
        return
    end
    local skill = SkillConfig:getSkillByItemId(itemId)
    if skill == nil then
        return
    end
    MsgSender.sendMsg(Messages:msgUseSkillHint(player.name, skill.tip))
    if skill.id == "1" then
        -- 闪现
        LogicListener.useFlashSkill(player)
        HostApi.sendPlaySound(player.rakssid, 302)
    end
    if skill.id == "2" then
        -- 加速
        LogicListener.useSpeedSkill(player)
        HostApi.sendPlaySound(player.rakssid, 307)
    end
    if skill.id == "3" then
        -- 隐身
        LogicListener.useInvisibilitySkill(player)
        HostApi.sendPlaySound(player.rakssid, 306)
    end
    if skill.id == "4" then
        -- 飞行
        LogicListener.useFlySkill(player)
        HostApi.sendPlaySound(player.rakssid, 303)
    end
    if skill.id == "5" then
        -- 无敌
        LogicListener.useInvincibleSkill(player)
        HostApi.sendPlaySound(player.rakssid, 305)
    end
    if skill.id == "6" then
        -- 传送
        LogicListener.useTeleportSkill()
        HostApi.sendPlaySound(player.rakssid, 308)
    end
    if skill.id == "7" then
        -- 黑暗
        LogicListener.useDarkSkill(player)
        HostApi.sendPlaySound(player.rakssid, 300)
    end
    player:clearInv()
end

function LogicListener.useFlashSkill(releaser)
    local position = releaser:getPosition()
    local yaw = releaser:getYaw()
    local pi = 3.1415
    local radian = (2 * pi / 360) * yaw
    for _ = 1, 5 do
        local targetX = position.x + math.sin(radian) * -1
        local targetZ = position.z + math.cos(radian) * 1
        if EngineWorld:getBlockId(VectorUtil.newVector3i(targetX, position.y, targetZ)) == 0 then
            position.x = targetX
            position.z = targetZ
        else
            break
        end
    end
    releaser:teleportPos(position)
end

function LogicListener.useSpeedSkill(releaser)
    releaser:addEffect(PotionID.MOVE_SPEED, 10, 1)
end

function LogicListener.useInvisibilitySkill(releaser)
    releaser:addEffect(PotionID.INVISIBILITY, 5, 1)
end

function LogicListener.useFlySkill(releaser)
    releaser:setFlying()
end

function LogicListener.useInvincibleSkill(releaser)
    releaser:setInvincible(true)
end

function LogicListener.useTeleportSkill()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife then
            player:teleportPos(GameConfig.gamePos)
        end
    end
end

function LogicListener.useDarkSkill(releaser)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife and player ~= releaser then
            player:showMask()
        end
    end
end

return LogicListener