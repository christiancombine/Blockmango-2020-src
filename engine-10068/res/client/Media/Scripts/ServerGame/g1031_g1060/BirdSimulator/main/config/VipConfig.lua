VipConfig = {}
VipConfig.vip = {}

function VipConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.value = tonumber(v.value)
        self.vip[data.id] = data
    end
end

function VipConfig:getVipValueById(id)
    if self.vip[id] ~= nil then
        return self.vip[id]
    end

    return 0
end

return VipConfig