BaseMessage = {}
---   指令列表
BaseMessage.OrderList = {}
---   聊天记录
BaseMessage.ChatRecord = {}

local function initChatRecord(name, text)
    local chat = {
        name = name,
        text = text,
    }
    return chat
end

local function initOrderRecord(orderIndex, text)
    local order = {
        index = orderIndex,
        text = text,
    }
    return order
end

function BaseMessage:addChatRecord(name, text)
    local record = initChatRecord(name, text)
    table.insert(self.ChatRecord, record)
    return #self.ChatRecord
end

function BaseMessage:addOrderList(teamId, orderId)
    local record = initOrderRecord(teamId, orderId)
    if self.OrderList[tostring(teamId)] == nil then
        self.OrderList[tostring(teamId)] = {}
    end
    table.insert(self.OrderList[tostring(teamId)], record)
    return #self.OrderList[tostring(teamId)]
end

return BaseMessage