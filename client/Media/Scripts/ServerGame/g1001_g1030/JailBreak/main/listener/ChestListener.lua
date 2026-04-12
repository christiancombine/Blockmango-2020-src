require "config.ChestConfig"
require "config.ScoreConfig"
require "config.StrongboxConfig"
require "Match"

ChestListener = {}
ChestListener.chest = {}

function ChestListener:init()
    ChestOpenEvent.registerCallBack(function(vec3, inv, rakssid)
        self:onChestOpen(vec3, inv, rakssid)
    end)
    FurnaceOpenEvent.registerCallBack(function ()
        return false
    end)
end

function ChestListener:onChestOpen(vec3, inv, rakssid)

    local player = PlayerManager:getPlayerByRakssid(rakssid)

    if player == nil then
        return
    end

    local config = ChestConfig:getChest(vec3)
    if config == nil then
        return
    end

    local sign = VectorUtil.hashVector3(vec3)
    local chest = self.chest[sign]
    if chest ~= nil then
        if os.time() - chest.initTime >= chest.resetTime then
            self.chest[sign] = nil
        end
    end

    if self.chest[sign] == nil then
        inv:clearChest()
        ChestConfig:fillItem(inv, config.type)
        self.chest[sign] = {}
        self.chest[sign].resetTime = math.random(config.resetTime[1], config.resetTime[2])
        self.chest[sign].initTime = os.time()
        self.chest[sign].players = {}

        if self.chest[sign].players[rakssid] == nil then
            player:addThreat(config.threat)
            player:subMerit(config.merit)
            player:addScore(ScoreConfig.criminalIncident)
            player:openChest(config.name)
            player:policeOpenChestMsg(config.name)
            self.chest[sign].players[rakssid] = true
        end
    end

end

return ChestListener