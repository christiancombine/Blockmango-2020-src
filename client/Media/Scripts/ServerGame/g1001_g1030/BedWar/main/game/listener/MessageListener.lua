MessageListener = {}

function MessageListener:init()
    SendShortMessageEvent:registerCallBack(self.onSendShortMessage)
    SendChatEvent:registerCallBack(self.onSendChat)
end

function MessageListener.onSendShortMessage(userId, commandId)
    local commandConfig = CommandSettingConfig:getConfigById(commandId)
    if not commandConfig then
        return
    end
    local sender = PlayerManager:getPlayerByUserId(userId)
    if not sender then
        return
    end
    sender:onSendCommand(commandConfig)
    MessagesManage:sendShortMessage(userId, commandId)
end

function MessageListener.onSendChat(userId, data)
    local builder = DataBuilder.new():fromData(data)
    local chatText = builder:getParam("text")
    local type = builder:getNumberParam("type")
    if not chatText then
        return
    end
    ---读配置，确定聊天的类型，决定发给哪些玩家
    local cmdText = ""
    if string.find(chatText, "/pangu") then
        cmdText = "/pangu" .. StringUtil.split(chatText, "/pangu", true)[2]
    end
    if HostApi.consumeCmd(userId, cmdText) then
        return
    end
    MessagesManage:sendChat(userId, chatText, type)
end

