local handler = class("CommonPacketHandler", EventObservable)

function handler:init()
    self:registerDataCallBack("OpenLotteryBox", self.onPlayerOpenLotteryBox)
    self:registerDataCallBack("UseMovieTicket", self.onPlayerUseMovieTicket)
end

function handler.onPlayerOpenLotteryBox(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local chestId = builder:getNumberParam("chestId")
    local num = builder:getNumberParam("num")
    GameChestLottery:onPlayerOpenLotteryChest(player, chestId, math.max(num, 1))
end

function handler.onPlayerUseMovieTicket(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    if player:subTicket(1) then
        local params = DataBuilder.new():fromData(data):getNumberParam("params")
        local type =DataBuilder.new():fromData(data):getNumberParam("type")
        CustomListener.onWatchAdvertFinished(player.rakssid, type, params, 1)
    end
end

handler:init()