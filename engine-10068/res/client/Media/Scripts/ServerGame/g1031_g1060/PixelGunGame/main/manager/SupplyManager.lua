require "data.SupplyPoint"

SupplyManager = {}
SupplyManager.SupplyPoints = {}
SupplyManager.WeaponSupply = {}

function SupplyManager:init()
    local count = SupplyConfig:getMaxSupplyCount()
    for i = 1, count do
        SupplyManager:randomSupply()
    end
end

function SupplyManager:isSupplyExist(id)
    for _, supply in pairs(self.SupplyPoints) do
        if supply.id == id then
            return true
        end
    end
    return false
end

function SupplyManager:getSupplyCount()
    local count = 0
    for _, supply in pairs(self.SupplyPoints) do
        count = count + 1
    end
    return count
end

function SupplyManager:randomSupply(time)
    local createFunc = function()
        if SupplyManager:getSupplyCount() >= SupplyConfig:getMaxSupplyCount() then
            return
        end
        local config = SupplyConfig:randomSupply()
        local times = 0
        while SupplyManager:isSupplyExist(config.id) and times < 50 do
            config = SupplyConfig:randomSupply()
            times = times + 1
        end
        SupplyPoint.new(config)
    end
    if time and time > 0 then
        LuaTimer:schedule(createFunc, time * 1000)
    else
        createFunc()
    end
end

function SupplyManager:onAddSupplyPoint(supply)
    self.SupplyPoints[tostring(supply.entityId)] = supply
end

function SupplyManager:onAddWeaponSupply(supply)
    self.WeaponSupply[tostring(supply.entityId)] = supply
end

function SupplyManager:onRemoveSupplyPoint(supply)
    self.SupplyPoints[tostring(supply.entityId)] = nil
end

function SupplyManager:onRemoveWeaponSupply(supply)
    self.WeaponSupply[tostring(supply.entityId)] = nil
end

function SupplyManager:onPlayerMove(player, x, y, z)
    local tempSupplyPoints = {}
    for _, supply in pairs(self.SupplyPoints) do
        table.insert(tempSupplyPoints, supply)
    end
    for _, supply in pairs(tempSupplyPoints) do
        supply:onPlayerMove(player, x, y, z)
    end

    local tempWeaponSupply  = {}
    for _, supply in pairs(self.WeaponSupply) do
        table.insert(tempWeaponSupply, supply)
    end
    for _, supply in pairs(tempWeaponSupply) do
        supply:onPlayerMove(player, x, y, z)
    end

    if GameMatch.Process:isRunning() then
        self:setClosestWeapon(player, x, y, z)
    end

end

function SupplyManager:setClosestWeapon(player, x, y, z)
    for _, supply in pairs(self.WeaponSupply) do
        local distance = supply:getDistance(x, y, z)
        local gun = GunConfig:getGunByItemId(supply.gunId)
        if gun.quality == GunConfig.Quality.Legend then
            if supply.entityId == player.closestWeapon.id and distance > GameConfig.weaponWarningRange then
                supply.inWarning = false
                player:resetClosestWeapon(-1, 5000)
            end
            if distance < player.closestWeapon.distance and distance <= GameConfig.weaponWarningRange then
                player:resetClosestWeapon(supply.entityId, distance)
            end
        end
    end

    if player.closestWeapon.id ~= -1 then
        local supply = self.WeaponSupply[tostring(player.closestWeapon.id)]
        if supply ~= nil then
            supply:setWarning(player)
        else
            player:resetClosestWeapon(-1, 5000)
        end
    end
end

function SupplyManager:playerReceiveWeapon(userId)
    for _, supply in pairs(self.SupplyPoints) do
        if supply.pickingPlayer and supply.pickingPlayer == userId then
            supply:onPlayerReceive(userId)
            return
        end
    end

    for _, supply in pairs(self.WeaponSupply) do
        if supply.pickingPlayer and supply.pickingPlayer == userId then
            supply:onPlayerReceive(userId)
            return
        end
    end
end
