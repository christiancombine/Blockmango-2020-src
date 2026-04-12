--region *.lua
require "config.GameConfig"
require "config.TeamConfig"
require "config.MoneyConfig"
require "config.EnchantMentNpcConfig"

local CGameTimeTask = class("GameTimeTask", ITask)

function CGameTimeTask:onTick(ticks)

    if (ticks >= GameConfig.gameTime) then
        self:stop()
    end

    local seconds = GameConfig.gameTime - ticks

    if ticks == GameConfig.enchantmentOpenTime then
        MsgSender.sendCenterTips(3, Messages:EnchantMentOpen())
        GameMatch.allowEnchantment = true
        EnchantMentNpcConfig:addLightColumnEffect()
    end

    if seconds >= 60 and seconds % 60 == 0 then
        MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    elseif seconds >= 60 and seconds % 30 == 0 then
        if seconds % 60 == 0 then
            MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
        else
            MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, true))
        end
    elseif seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameEndTimeHint(seconds))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    --self:sendGameData()

    self:addMoneyToWorld(ticks)

    self:teleportStandTime()

    self.tick = ticks

end

function CGameTimeTask:onCreate()
    GameMatch.curStatus = self.status
end

function CGameTimeTask:teleportStandTime()
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player.entityPlayerMP ~= nil then
            local currentPosition = player.entityPlayerMP:getBottomPos()
            if player.lastPosition ~= nil and VectorUtil.equal(player.lastPosition, currentPosition) then
                if os.time() - player.standTime >= GameConfig.standTime then
                    if player.isLife or player.realLife then
                        player:teleInitPos()
                        player.standTime = os.time()
                    end
                end
            else
                player.standTime = os.time()
                player.lastPosition = {}
                player.lastPosition.x = currentPosition.x
                player.lastPosition.y = currentPosition.y
                player.lastPosition.z = currentPosition.z
            end
        end
    end
end

function CGameTimeTask:addMoneyToWorld(ticks)
    for i, v in pairs(GameMatch.money) do
        if ticks % v.mineral.time == 0 then
            EngineWorld:addEntityItem(v.mineral.itemId, v.mineral.num, 0, v.mineral.life, v.position)
        end
    end
end

function CGameTimeTask:sendGameData()
    local msg = ""
    for i, v in pairs(GameMatch.Teams) do
        msg = msg .. v:getDisplayStatus() .. TextFormat.colorWrite .. " - "
    end
    if #msg ~= 0 then
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            local pmsg = msg .. v.team.color .. " T : " .. v.team.name ..
            TextFormat.colorWrite .. " - " .. TextFormat.colorGold .. " Score : " .. v.score .. TextFormat.colorEnd
            MsgSender.sendBottomTipsToTarget(v.rakssid, 3, pmsg)
        end
    end
end

function CGameTimeTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:endMatch()
end

GameTimeTask = CGameTimeTask.new(3)

return GameTimeTask

--endregion
