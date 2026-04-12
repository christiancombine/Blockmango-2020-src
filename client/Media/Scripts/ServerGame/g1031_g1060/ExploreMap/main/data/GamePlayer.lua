---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
    self:initStaticAttr(0)
    self:setAllowFlyingOn(true)
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
end

function GamePlayer:teleInitPos()
    self:teleportPosByRecord(GameConfig.initPos, 0)
end

function GamePlayer:reward()

end

function GamePlayer:setAllowFlyingOn(sign)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setAllowFlying(sign)
    end
end

function GamePlayer:teleportPosByRecord(target_pos, yaw)
    HostApi.sendPlaySound(self.rakssid, 141)
    self:teleportPosWithYaw(target_pos, yaw)
end

return GamePlayer

--endregion


