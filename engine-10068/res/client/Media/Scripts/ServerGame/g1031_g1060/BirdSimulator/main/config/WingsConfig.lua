WingsConfig = {}
WingsConfig.wings = {}

function WingsConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.wingId)
        data.speed = tonumber(v.speed)
        data.reduction = tonumber(v.reduction)
        self.wings[data.id] = data
    end
end

function WingsConfig:getWingInfo(id)
    if self.wings[id] ~= nil then
        return self.wings[id]
    end

   return nil
end


return WingsConfig