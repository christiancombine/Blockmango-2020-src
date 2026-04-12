ShopHouseConfig = {}
ShopHouseConfig.mainShop = {}
ShopHouseConfig.branchItems = {}

function ShopHouseConfig:sortDataASC(data1, data2)
    data1 = tonumber(data1)
    data2 = tonumber(data2)
    if data1 >= data2 then
        return data2, data1
    end
    return data1, data2
end

function ShopHouseConfig:initBranchPos(v)
    local minX, maxX = self:sortDataASC(v.x1, v.x2)
    local minY, maxY = self:sortDataASC(v.y1, v.y2)
    local minZ, maxZ = self:sortDataASC(v.z1, v.z2)

    local pos = {}
    pos.min = VectorUtil.newVector3(minX, minY, minZ)
    pos.max = VectorUtil.newVector3(maxX, maxY, maxZ)

    return pos
end

function ShopHouseConfig:initShopHouses(configs)
    local data = {}
    for _, v in pairs(configs) do
        local classifyId = tonumber(v.classifyId)
        local classify = v.classify
        local classifyImage = (v.classifyImage ~= "@@@" and {v.classifyImage} or {""})[1]
        local branchId = tonumber(v.branchId)
        local areaId = tonumber(v.areaId)

        if not data[classifyId] then
            data[classifyId] = {
                classifyId = classifyId,
                classify = classify,
                classifyImage = classifyImage,
                branch = {},
            }
        end

        if not data[classifyId].branch[branchId] then
            data[classifyId].branch[branchId] = {
                itemsInfos = {},
                areaId = {},
                pos = {},
            }
        end

        table.insert(data[classifyId].branch[branchId].pos, self:initBranchPos(v))
        table.insert(data[classifyId].branch[branchId].areaId, areaId)
    end
    self.mainShop = data
end

function ShopHouseConfig:initShopHouseItems(configs)
    local items = {}
    for _, v in pairs(configs) do
        local val = {}
        val.id = tonumber(v.id)
        val.itemId = tonumber(v.ItemId)
        val.actorModelName = v.actorModelName
        val.icon = v.icon
        val.actorBody = v.actorBody
        val.actorBodyId = tonumber(v.actorBodyId)
        val.itemName = v.ItemName
        val.pos = VectorUtil.newVector3(v.x, v.y, v.z)
        val.yaw = tonumber(v.yaw)
        val.itemPrice = tonumber(v.ItemPrice)
        val.moneyType = tonumber(v.MoneyType)
        val.itemType = tonumber(v.ItemType)
        val.itemDesc = tonumber(v.ItemDesc)
        val.branchId = tonumber(v.branchId)
        val.actorName = v.actorName
        val.actorId = v.actorId
        val.iconNew = v.iconNew
        val.scale = tonumber(v.scale)
        val.isCanBuy = tonumber(v.isCanBuy)
        val.limit = tonumber(v.limit)
        val.availableTime = tonumber(v.availableTime)

        if not items[val.branchId] then
            items[val.branchId] = {}
        end
        table.insert(items[val.branchId], val)
    end
    self.branchItems = items
end

function ShopHouseConfig:getAllShopHouses()
    return self.mainShop
end

function ShopHouseConfig:getBranchItemsByBranchId(branchId)
    return self.branchItems[tonumber(branchId)] or {}
end
