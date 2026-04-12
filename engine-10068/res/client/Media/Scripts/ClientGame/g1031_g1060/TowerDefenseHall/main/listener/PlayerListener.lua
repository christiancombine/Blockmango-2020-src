
PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)

    Listener.registerCallBack(RenderPlayerShowNameEvent, self.onRenderPlayerShowName)
    Listener.registerCallBack(UpdateRankKeyEvent, self.onUpdateRankKey)

    self.timerKey = LuaTimer:scheduleTimer(self.checkPlayersMove, 100)

end

function PlayerListener.checkPlayersMove()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        local pos = player:getPosition()
        AreaManager:onPlayerMove(player, pos.x, pos.y, pos.z)
    end
end

function PlayerListener.onPlayerLogout(player)
    AreaManager:onPlayerMove(player, 0, 99999, 0)
end

function PlayerListener.onUpdateRankKey(key)
    RankConfig:updateKey(tonumber(key))
end

function PlayerListener.onRenderPlayerShowName(pPlayer, actor, position)
    if not pPlayer or not actor then
        return true
    end
    pPlayer:setFloatProperty("PlayerShowNameOffset",  (pPlayer:getHeight() + 0.1) * (actor:GetTotalScale().y) - (pPlayer:getYOffset() + 0.25))
    pPlayer:setFloatProperty("PlayerShowNameScale",  0.4 * pPlayer:getHeight())
    return false
end