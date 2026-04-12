HardnessConfig = {}
HardnessConfig.hardness = {}

function HardnessConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.hardness = tonumber(v.hardness)
        self.hardness[data.id] = data
    end
end

function HardnessConfig:prepareBlockHardness()
    for _, v in pairs(self.hardness) do
        HostApi.setBlockAttr(v.id, v.hardness)
    end
end

return HardnessConfig