ChestConfig = {}
ChestConfig.area = {}
ChestConfig.chestTeam = {}
ChestConfig.chest = {}
ChestConfig.hasOpenChest = {}
ChestConfig.tempChestTeamId = {}
--ChestConfig.index = 1

function ChestConfig:initChestArea(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.startX = tonumber(v.startX)
        data.startY = tonumber(v.startY)
        data.startZ = tonumber(v.startZ)
        data.endX = tonumber(v.endX)
        data.endY = tonumber(v.endY)
        data.endZ = tonumber(v.endZ)
        data.chestTeamId = tonumber(v.chestTeamId)
        self.area[data.id] = data
    end
end

function ChestConfig:initChestTeam(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.chestTeamId = tonumber(v.chestTeamId)
        data.chestId = tonumber(v.chestId)
        data.weight = tonumber(v.weight)
        data.isChoose = false
        self.chestTeam[data.id] = data
    end
end

function ChestConfig:initChest(config)
    for i, v in pairs(config) do
        if self.chest[v.chestId] == nil then
            self.chest[v.chestId] = {}
        end
        local data = {}
        data.id = tonumber(v.id)
        data.chestId = tonumber(v.chestId)
        data.itemId = tonumber(v.itemId)
        data.num = tonumber(v.num)
        data.meta = tonumber(v.meta)
        data.enchantId = StringUtil.split(v.enchantId, "#")
        data.enchantLevel = StringUtil.split(v.enchantLevel, "#")
        self.chest[v.chestId][data.id] = data
    end
end

function ChestConfig:randomArea(area)
    local x = math.random(math.min(area.startX, area.endX), math.max(area.startX, area.endX))
    --local y = math.random(math.min(area.startY, area.endY), math.max(area.startY, area.endY))
    local z = math.random(math.min(area.startZ, area.endZ), math.max(area.startZ, area.endZ))

    local max = math.max(area.startY, area.endY)
    local min = math.min(area.startY, area.endY)
    for y = max, min, -1 do
        local id = EngineWorld:getBlockId(VectorUtil.newVector3i(x, y, z))
        --HostApi.log("x = " .. x .. ", y = " .. y .. ", z = " .. z)
        if id ~= 0 and id ~= 54 then
            local prev_Id1 = EngineWorld:getBlockId(VectorUtil.newVector3i(x, y + 1, z))
            local prev_Id2 = EngineWorld:getBlockId(VectorUtil.newVector3i(x, y + 2, z))
            if prev_Id1 == 0 and prev_Id2 == 0 then
                local vec3 = VectorUtil.newVector3i(x, y + 1, z)
                local sign = VectorUtil.hashVector3(vec3)
                self.tempChestTeamId[sign] = area.chestTeamId
                --HostApi.log(self.index .. ": x = " .. vec3.x .. ", y = " .. vec3.y .. ", z = " .. vec3.z)
                --self.index = self.index + 1
                return vec3
            end
        end
    end

    return self:randomArea(area)
end


function ChestConfig:prepareChest()
    for _, area in pairs(self.area) do
        local initPos = self:randomArea(area)
        EngineWorld:setBlock(initPos, 54)
    end
end

function ChestConfig:randomChestId(chestTeamId, weight)
    math.randomseed(tostring(os.time()):reverse():sub(1, 7))
    local randomNum = math.random(0, weight)
    local num = 0
    for i, v in pairs(self.chestTeam) do
        if v.chestTeamId == chestTeamId then
            if v.isChoose == false then
                if randomNum >= num and randomNum <= num + v.weight then
                    v.isChoose = true
                    return v.chestId
                end
                num = num + v.weight
            end
        end
    end
    return 0
end

function ChestConfig:randomChestInventory(inv, chestTeamId)
    local totalWeight = 0
    for _, chestTeam in pairs(self.chestTeam) do
        if chestTeam.chestTeamId == chestTeamId and chestTeam.isChoose == false then
            totalWeight = totalWeight + chestTeam.weight
        end
    end

    local chestId = self:randomChestId(chestTeamId, totalWeight)
    local itemSlot = 0
    for _, v in pairs(self.chest) do
        for _, chest in pairs(v) do
            if chest.chestId == chestId then
                ChestConfig:fillChestByChestId(inv, chest, itemSlot)
                itemSlot = itemSlot + 1
            end
        end
    end
end

function ChestConfig:fillChestByChestId(inv, chest, itemSlot)
    local enchantments = {}
    local isEnchant = false
    local index = 1
    for i, level in pairs(chest.enchantLevel) do
        if tonumber(level) ~= 0 then
            isEnchant = true
            for j, enchantId in pairs(chest.enchantId) do
                if i == j then
                    local enchantment = {}
                    enchantment[1] = enchantId
                    enchantment[2] = level
                    enchantments[index] = enchantment
                    index = index + 1
                end
            end
        end
    end

    if isEnchant == true then
        inv:addEchantmentItem(itemSlot, chest.itemId, chest.num, chest.meta, enchantments)
    else
        inv:initByItem(itemSlot, chest.itemId, chest.num, chest.meta)
    end
end

function ChestConfig:fillChest(vec3, inv)
    local sign = VectorUtil.hashVector3(vec3)
    if self.hasOpenChest[sign] == nil then
        if self.tempChestTeamId[sign] ~= nil then
            inv:clearChest()
            ChestConfig:randomChestInventory(inv, self.tempChestTeamId[sign])
            self.tempChestTeamId[sign] = nil
        end

        self.hasOpenChest[sign] = true
    end
end


return ChestConfig