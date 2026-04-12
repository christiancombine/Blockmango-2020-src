---
--- Created by Jimmy.
--- DateTime: 2018/6/13 0013 11:00
---
PropBlockConfig = {}
PropBlockConfig.blocks = {}

function PropBlockConfig:init(blocks)
    self:initBlocks(blocks)
end

function PropBlockConfig:initBlocks(blocks)
    for id, block in pairs(blocks) do
        local item = {}
        item.id = tonumber(block.ID)
        item.blockId = tonumber(block.blockId)
        item.schematic = {}
        table.insert(item.schematic, block.schemetic1)
        table.insert(item.schematic, block.schemetic2)
        table.insert(item.schematic, block.schemetic3)
        table.insert(item.schematic, block.schemetic4)
        table.insert(item.schematic, block.schemetic5)
        table.insert(item.schematic, block.schemetic6)
        table.insert(item.schematic, block.schemetic7)
        table.insert(item.schematic, block.schemetic8)
        item.groupId = tonumber(block.groupId)

        item.offset45X = 0
        item.offset45Z = 0
        item.rotation45X = true
        item.rotation45Z = false
        local offset45 = StringUtil.split(tostring(block.offset45), "#")
        if offset45 then
            item.offset45X = tonumber(offset45[1])
            item.offset45Z = tonumber(offset45[2])
        end

        item.offset135X = 0
        item.offset135Z = 0
        item.rotation135X = true
        item.rotation135Z = true
        local offset135 = StringUtil.split(block.offset135, "#")
        if offset135 then
            item.offset135X = tonumber(offset135[1])
            item.offset135Z = tonumber(offset135[2])
        end

        item.offset225X = 0
        item.offset225Z = 0
        item.rotation225X = false
        item.rotation225Z = true
        local offset225 = StringUtil.split(block.offset225, "#")
        if offset225 then
            item.offset225X = tonumber(offset225[1])
            item.offset225Z = tonumber(offset225[2])
        end

        item.offset315X = 0
        item.offset315Z = 0
        item.rotation315X = false
        item.rotation315Z = false
        local offset315 = StringUtil.split(block.offset315, "#")
        if offset315 then
            item.offset315X = tonumber(offset315[1])
            item.offset315Z = tonumber(offset315[2])
        end

        self.blocks[id] = item
    end
end

function PropBlockConfig:isPropBlock(blockId)
    for id, block in pairs(self.blocks) do
        if tonumber(block.blockId) == tonumber(blockId) then
            return true
        end
    end

    return false
end

function PropBlockConfig:getSchematicFile(blockId, teamId)
    for _, block in pairs(self.blocks) do
        if tonumber(block.blockId) == tonumber(blockId) then
            if teamId ~= nil then
                return tostring(block.schematic[teamId])
            else
                return nil
            end
        end
    end
    return nil
end

function PropBlockConfig:getOffset(blockId, angle)
    if angle < 0 then
        while true do
            angle = angle + 360.0
            if angle >= 0 then
                break
            end
        end
    elseif angle >= 360.0 then
        while true do
            angle = angle - 360.0
            if angle < 360.0 then
                break
            end
        end
    else

    end

    for id, block in pairs(self.blocks) do
        if tonumber(block.blockId) == tonumber(blockId) then
            if angle >= 45 and angle < 135 then
                return block.offset45X, block.offset45Z, block.rotation45X, block.rotation45Z
            elseif angle >= 135 and angle < 225 then
                return block.offset135X, block.offset135Z, block.rotation135X, block.rotation135Z
            elseif angle >= 225 and angle < 315 then
                return block.offset225X, block.offset225Z, block.rotation225X, block.rotation225Z
            else
                return block.offset315X, block.offset315Z, block.rotation315X, block.rotation315Z
            end
        end
    end
    return 0, 0, false, false
end

function PropBlockConfig:getOffsetById(configId, angle)
    for _, block in pairs(self.blocks) do
        if tonumber(block.id) == tonumber(configId) then
            if angle >= 45 and angle < 135 then
                return block.offset45X, block.offset45Z, block.rotation45X, block.rotation45Z
            elseif angle >= 135 and angle < 225 then
                return block.offset135X, block.offset135Z, block.rotation135X, block.rotation135Z
            elseif angle >= 225 and angle < 315 then
                return block.offset225X, block.offset225Z, block.rotation225X, block.rotation225Z
            else
                return block.offset315X, block.offset315Z, block.rotation315X, block.rotation315Z
            end
        end
    end
    return 0, 0, false, false
end

function PropBlockConfig:getSchematicByGroupId(id)
    local list = {}
    for i, v in pairs(PropBlockConfig.blocks) do
        if v then
            if tonumber(v.groupId) == tonumber(id) then
                table.insert(list, v)
            end
        end
    end
    return list
end

local function createBedProtect(teamId)
    local teamConfig = TeamConfig:getTeam(teamId)
    local list = PropBlockConfig:getSchematicByGroupId(teamConfig.schemeticGroupId)
    if #list ~= 0 then
        local toPlacePos = TableUtil.copyTable(teamConfig.bedPos[1])
        local yaw = teamConfig.bedYaw
        local propBlockConfig = list[math.random(1, #list)]
        if propBlockConfig then
            local schematic = propBlockConfig.schematic[teamId]
            local offsetX, offsetZ, rotationX, rotationZ = PropBlockConfig:getOffsetById(propBlockConfig.id, yaw)
            if schematic ~= nil and #schematic > 10 then
                toPlacePos.x = toPlacePos.x + offsetX
                toPlacePos.z = toPlacePos.z + offsetZ
                EngineWorld:createSchematic(schematic, toPlacePos, rotationX, rotationZ, false, 0, 0, 0, 1)
            end
        end
    end
end

function PropBlockConfig:createBedProtect()
    if GameType == "g1008" then
        for teamId, team in pairs(GameMatch.Teams) do
            local AIPlayers = AIManager:getAIsByTeamId(teamId)
            if #AIPlayers > 0 and not team:isDeath() then
                createBedProtect(teamId)
            end
        end
    else
        for teamId, team in pairs(GameMatch.Teams) do
            if not team:isDeath() then
                createBedProtect(teamId)
            end
        end
    end
end