GuideConfig = {}
GuideConfig.Guides = {}
GuideConfig.Conditions = {}
GuideConfig.Events = {}
GuideConfig.TriggerType = {
    GuideFinish = 4
}
GuideConfig.EventType = {
    FlashTo = 2
}

function GuideConfig:initGuide(guides)
    for _, config in pairs(guides) do
        local item = {}
        item.Id = tonumber(config.Id)
        item.ResetId = tonumber(config.ResetId)
        item.TriggerId = tonumber(config.TriggerId)
        item.EventId = tonumber(config.EventId)
        item.Lv = tonumber(config.Lv)
        self.Guides[item.Id] = item
    end
end

function GuideConfig:initCondition(conditions)
    for _, condition in pairs(conditions) do
        if tonumber(condition.Type) ~= self.TriggerType.GuideFinish then
            local item = {}
            item.Id = tonumber(condition.Id)
            item.Type = tonumber(condition.Type)
            self.Conditions[item.Id] = item
        end
    end
end

function GuideConfig:initEvent(events)
    for _, event in pairs(events) do
        local item = {}
        item.Id = tonumber(event.Id)
        item.Type = tonumber(event.Type)
        item.Pos = StringUtil.split(event.Pos, "#")
        self.Events[item.Id] = item
    end
end

function GuideConfig:isGuideFirstOfChain(guideId)
    local config = self:getGuideConfigById(guideId)
    if not config then
        return false
    end
    for _, condition in pairs(self.Conditions) do
        if condition.Id == config.TriggerId then
            return true
        end
    end
    return false
end

function GuideConfig:getEventByGuideId(guideId)
    local config = self:getGuideConfigById(guideId)
    if not config then
        return nil
    end
    for _, event in pairs(self.Events) do
        if event.Id == config.EventId then
            return event
        end
    end
    return nil
end

function GuideConfig:getGuideConfigById(guideId)
    return self.Guides[guideId]
end

return GuideConfig