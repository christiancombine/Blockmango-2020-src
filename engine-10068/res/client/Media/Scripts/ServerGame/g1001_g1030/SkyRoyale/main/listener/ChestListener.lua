
require "Match"

ChestListener = {}

function ChestListener:init()
    ChestOpenEvent.registerCallBack(function(vec3, inv, rakssid)
        self:onChestOpen(vec3, inv, rakssid)
    end)
end

function ChestListener:onChestOpen(vec3, inv, rakssid)
    ChestConfig:fillChest(vec3, inv)
end

return ChestListener