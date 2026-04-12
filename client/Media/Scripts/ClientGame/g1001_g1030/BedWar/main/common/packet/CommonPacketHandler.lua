local handler = class("CommonPacketHandler", EventObservable)

function handler:init()
    self:registerDataCallBack("UseActionSuccess", function(data)
        self.onOtherPlayerUseAction(data)
    end, DataBuilderProcessor)

    self:registerDataCallBack("TicketNum", function(data)
        local num = data:getNumberParam("num")
        PlayerWallet:setMoneyCount(Define.Money.TICKET, num)
    end, DataBuilderProcessor)

end

function handler.onOtherPlayerUseAction(data)
    local serverId = data:getNumberParam("serverId")
    local actionId = data:getNumberParam("actionId")
    local otherPlayer = PlayerManager:getPlayerByServerId(serverId)

    local entityRender = EntityRenderManager.Instance()
    if not entityRender then
        return
    end
    if otherPlayer and otherPlayer.userId ~= PlayerManager:getClientPlayer().userId then
        local config = ActionEmojiConfig:getConfig(actionId)
        if config then
            otherPlayer:setPlayerActionName(tostring(config.name))
            otherPlayer:setUpperAction1(1012)
        end
    end
end

handler:init()