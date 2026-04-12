require "game.ui.window.GUICommandItem"

local UICommandList = class("UICommandList", IGUILayout)

function UICommandList:onLoad()
    self.llCommandList = self:getChildWindow("BedWarCommandList-list", GUIType.Layout)
    self:initRegister()
    self:initCommandList()
end

function UICommandList:initCommandList()
    local commandConfig = CommandSettingConfig:getAllConfig()
    self.commandList = {}

    for _, command in pairs(commandConfig) do
        table.insert(self.commandList, { id = command.id, canUse = command.price == 0, icon = command.icon })
    end
    self.gvCommandItems = IGUIGridView.new("BedWarCommandList-Item", self.llCommandList)
    self.gvCommandItems:setArea({ 0, 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 })
    self.gvCommandItems:setConfig(16, 14, 3)
    local ContentSize = (self.llCommandList:GetPixelSize().x - 32) / 3
    self.commandAdapter = (require "game.ui.adapter.CommandAdapter").new()
    self.commandAdapter:setItemSize(ContentSize, ContentSize)
    self.gvCommandItems:setAdapter(self.commandAdapter)
    self.commandAdapter:setData(self.commandList)
end

function UICommandList:initRegister()
    self.root:registerEvent(GUIEvent.TouchDown, function()
        self:hide()
    end)
    self:registerDataCallBack("commandList", function(data)
        self:initCommandListData(data)
    end, JsonBuilderProcessor)
end

function UICommandList:initCommandListData(data)
    print_lua_table(data)
    for _, commandId in pairs(data) do
        self:onCommandStatusChange(commandId)
    end
    self.commandAdapter:setData(self.commandList)
end

function UICommandList:onCommandStatusChange(messageId)
    for _, command in pairs(self.commandList) do
        if command.id == messageId then
            command.canUse = true
            break
        end
    end
end

function UICommandList:onHide()
    GUIMain.cbShortMessage:SetChecked(false)
end

return UICommandList
