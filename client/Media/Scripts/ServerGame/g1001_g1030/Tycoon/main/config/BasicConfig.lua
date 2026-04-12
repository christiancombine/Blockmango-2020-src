BasicConfig = {}
BasicConfig.basicNum = 0
BasicConfig.basicPoints = {}

function BasicConfig:init(points)
    self:initBasics(points)
end

function BasicConfig:initBasics(config)
    for i, b_item in pairs(config) do
        local item = {}
        item.id = tonumber(b_item.id)
        item.basicPos = VectorUtil.newVector3(b_item.x, b_item.y, b_item.z)
        item.metaPos = VectorUtil.newVector3i(b_item.metaX, b_item.metaY, b_item.metaZ)
        item.xImage = tonumber(b_item.xImage) == 1
        item.zImage = tonumber(b_item.zImage) == 1
        item.yaw = tonumber(b_item.yaw)
        self.basicPoints[tostring(b_item.id)] = item
        BasicConfig.basicNum = BasicConfig.basicNum + 1
    end
end

function BasicConfig:getPosViByBasic(id, pos)
    local basic = self.basicPoints[tostring(id)]
    if basic ~= nil then
        local m_pos = VectorUtil.newVector3i((basic.basicPos.x + pos.x), (basic.basicPos.y + pos.y), (basic.basicPos.z + pos.z))
        return m_pos
    end
    return pos
end

function BasicConfig:getPosByBasic(id, pos)
    local basic = self.basicPoints[tostring(id)]
    local x = basic.basicPos.x
    local y = basic.basicPos.y
    local z = basic.basicPos.z
    local width = 22
    local length = 28
    if basic ~= nil then
        if basic.xImage and basic.zImage then
            local m_pos = VectorUtil.newVector3((x + length - pos.x + 1), (y + pos.y), (z + width - pos.z))
            return m_pos
        elseif basic.xImage == false and basic.zImage then
            local m_pos = VectorUtil.newVector3((x + pos.z), (y + pos.y), (z + length - pos.x + 1))
            return m_pos
        elseif basic.xImage and basic.zImage == false then
            local m_pos = VectorUtil.newVector3((x + width - pos.z), (y + pos.y), (z + pos.x))
            return m_pos
        else
            local m_pos = VectorUtil.newVector3((x + pos.x), (y + pos.y), (z + pos.z))
            return m_pos
        end
    end
    return pos
end

function BasicConfig:getPosiByBasic(id, pos)
    local basic = self.basicPoints[tostring(id)]
    local x = basic.basicPos.x
    local y = basic.basicPos.y
    local z = basic.basicPos.z
    local width = 21
    local length = 28
    if basic ~= nil then
        if basic.xImage and basic.zImage then
            local m_pos = VectorUtil.newVector3i((x + length - pos.x + 1), (y + pos.y), (z + width - pos.z))
            return m_pos
        elseif basic.xImage == false and basic.zImage then
            local m_pos = VectorUtil.newVector3i((x + pos.z), (y + pos.y), (z + length - pos.x))
            return m_pos
        elseif basic.xImage and basic.zImage == false then
            local m_pos = VectorUtil.newVector3i((x + width - pos.z - 1), (y + pos.y), (z + pos.x))
            return m_pos
        else
            local m_pos = VectorUtil.newVector3i((x + pos.x), (y + pos.y), (z + pos.z))
            return m_pos
        end
    end
    return pos
end

function BasicConfig:getPosiByBasicAndLW(id, pos, L, W)
    local basic = self.basicPoints[tostring(id)]
    local x = basic.basicPos.x
    local y = basic.basicPos.y
    local z = basic.basicPos.z
    local width = 21
    local length = 28
    if basic ~= nil then
        if basic.xImage and basic.zImage then
            local m_pos = VectorUtil.newVector3i((x + length - pos.x), (y + pos.y), (z + width - pos.z))
            return m_pos
        elseif basic.xImage == false and basic.zImage then
            local m_pos = VectorUtil.newVector3i((x + pos.z), (y + pos.y), (z + length - pos.x))
            return m_pos
        elseif basic.xImage and basic.zImage == false then
            local m_pos = VectorUtil.newVector3i((x + width - pos.z - 1) - (W or 0), (y + pos.y), (z + pos.x))
            return m_pos
        else
            local m_pos = VectorUtil.newVector3i((x + pos.x), (y + pos.y), (z + pos.z))
            return m_pos
        end
    end
    return pos
end

function BasicConfig:getMetaPosByBasic(id)
    local basic = self.basicPoints[tostring(id)]
    if basic ~= nil then
        return basic.metaPos
    end
    error("no can basic by" .. id)
end

function BasicConfig:getYawByBasic(id, yaw)
    local basic = self.basicPoints[tostring(id)]
    if basic ~= nil then
        local m_yaw = basic.yaw + yaw
        return m_yaw
    end
    return yaw
end

function BasicConfig:getXandZImageByBasics(id)
    local basic = self.basicPoints[tostring(id)]
    if basic ~= nil then
        return basic.xImage, basic.zImage
    end
    return false, false
end

function BasicConfig:getAreaByBasic(id, area)
    local basic = self.basicPoints[tostring(id)]
    local x = basic.basicPos.x
    local z = basic.basicPos.z
    local y = basic.basicPos.y
    local width = 21
    local length = 28
    if basic ~= nil then
        if basic.xImage and basic.zImage then
            local m_area = {}
            local s_area = { x + length - area[1][1], area[1][2] + y, z + width - area[1][3] }
            table.insert(m_area, s_area)
            local e_area = { x + length - area[2][1], area[2][2] + y, z + width - area[2][3] }
            table.insert(m_area, e_area)
            return m_area
        elseif basic.xImage == false and basic.zImage then
            local m_area = {}
            local s_area = { x + area[1][3], area[1][2] + y, z + length - area[1][1] }
            table.insert(m_area, s_area)
            local e_area = { x + area[2][3], area[2][2] + y, z + length - area[2][1] }
            table.insert(m_area, e_area)
            return m_area
        elseif basic.xImage and basic.zImage == false then
            local m_area = {}
            local s_area = { x + width - area[1][3], area[1][2] + y, area[1][1] + z }
            table.insert(m_area, s_area)
            local e_area = { x + width - area[2][3], area[2][2] + y, area[2][1] + z }
            table.insert(m_area, e_area)
            return m_area
        else
            local m_area = {}
            local s_area = { area[1][1] + x, area[1][2] + y, area[1][3] + z }
            table.insert(m_area, s_area)
            local e_area = { area[2][1] + x, area[2][2] + y, area[2][3] + z }
            table.insert(m_area, e_area)
            return m_area
        end
    end
    return area
end

function BasicConfig:getResourceGetPosByBasic(id, pos)
    local basic = self.basicPoints[tostring(id)]
    if basic ~= nil then
        if basic.xImage and basic.zImage then
            local m_pos = VectorUtil.newVector3i(pos.x, pos.y - 1, pos.z - 1)
            return m_pos
        elseif basic.xImage == false and basic.zImage then
            local m_pos = VectorUtil.newVector3i(pos.x + 1, pos.y - 1, pos.z)
            return m_pos
        elseif basic.xImage and basic.zImage == false then
            local m_pos = VectorUtil.newVector3i(pos.x - 1, pos.y - 1, pos.z)
            return m_pos
        else
            local m_pos = VectorUtil.newVector3i(pos.x, pos.y - 1, pos.z + 1)
            return m_pos
        end
    end
end

function BasicConfig:getResourceGetPosiByBasic(id, pos)
    local basic = self.basicPoints[tostring(id)]
    if basic ~= nil then
        if basic.xImage and basic.zImage then
            local m_pos = VectorUtil.newVector3i(pos.x, pos.y - 1, pos.z)
            return m_pos
        elseif basic.xImage == false and basic.zImage then
            local m_pos = VectorUtil.newVector3i(pos.x, pos.y - 1, pos.z)
            return m_pos
        elseif basic.xImage and basic.zImage == false then
            local m_pos = VectorUtil.newVector3i(pos.x, pos.y - 1, pos.z)
            return m_pos
        else
            local m_pos = VectorUtil.newVector3i(pos.x, pos.y - 1, pos.z)
            return m_pos
        end
    end
end

return BasicConfig