MessagesManage = {}

---转发聊天信息
function MessagesManage:sendChat(userId, text, sendObject)
    if sendObject == Define.MessageType.TeamChat then
        ------------------ 发给队友 -------------------
        local player = PlayerManager:getPlayerByUserId(userId)
        local players = PlayerManager:getPlayers()
        if player == nil then
            return
        end
        for _, otherPlayer in pairs(players) do
            if otherPlayer and player.teamId == otherPlayer.teamId then
                GamePacketSender:sendBedWarChat(otherPlayer.rakssid, userId, text)
            end
        end
    elseif sendObject == Define.MessageType.AllChat then
        GamePacketSender:sendBedWarChatToAll(userId, text)
    end
end

---转发快捷聊天
function MessagesManage:sendShortMessage(userId, ShortMessageId)
    ------------------ 发给队友 -------------------
    local player = PlayerManager:getPlayerByUserId(userId)
    local players = PlayerManager:getPlayers()
    if player == nil then
        return
    end
    for _, otherPlayer in pairs(players) do
        if otherPlayer and player.teamId == otherPlayer.teamId then
            GamePacketSender:sendBedWarShortMessage(otherPlayer.rakssid, userId, ShortMessageId)
        end
    end
end

---系统消息
function MessagesManage:sendSystemTipsToPlayer(rakssid, tipType, tipText)
    GamePacketSender:sendBedWarSystemTips(rakssid, tipType, tipText)
end

function MessagesManage:sendSystemTipsToTeam(teamId, tipType, tipText)
    ------------------ 发给队友 -------------------
    local players = PlayerManager:getPlayers()
    for _, otherPlayer in pairs(players) do
        if otherPlayer and teamId == otherPlayer.teamId then
            GamePacketSender:sendBedWarSystemTips(otherPlayer.rakssid, tipType, tipText)
        end
    end
end

function MessagesManage:sendSystemTipsToAll(tipType, tipText)
    GamePacketSender:sendBedWarSystemTipsToAll(tipType, tipText)
end

return MessagesManage