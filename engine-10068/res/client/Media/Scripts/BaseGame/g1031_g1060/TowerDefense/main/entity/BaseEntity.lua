BaseEntity = class("BaseEntity")

function BaseEntity:init()
    self.properties = TableUtil.copyTable(BuffConfig.initProperties)
    self.buffList = {}
end

function BaseEntity:changeAuraBuff(buffIds)
    local buffChangeList = TableUtil.copyTable(self.oldBuffIds or {})
    for buffId, count in pairs(buffIds) do
        buffChangeList[buffId] = (buffChangeList[buffId] or 0) - count
    end
    self.oldBuffIds = buffIds
    for buffId, count in pairs(buffChangeList) do
        local config = BuffConfig:getConfigById(tonumber(buffId))
        if count > 0 then
            for _ = 1, count do
                local list = self.buffList[tostring(config.classId)]
                if list then
                    local buff = list[1]
                    if buff and tonumber(buff.config.Id) == tonumber(buffId) then
                        buff:remove()
                    end
                end
            end
        end
    end
    for buffId, count in pairs(buffChangeList) do
        if count < 0 then
            for _ = 1, -count do
                GameBuff.new(self, tonumber(buffId))
            end
        end
    end
end

function BaseEntity:checkPropertyChange()

end

function BaseEntity:addEffect(name, effect, duration, scale)
    if isClient and self.entity and GUIMain.openBuff then
        self.entity:addCustomEffect(name, effect, duration, scale)
        return name
    end
end

function BaseEntity:removeEffect(name, effect)
    if isClient and self.entity then
        self.entity:addCustomEffect(name, effect or "", 0)
    end
end