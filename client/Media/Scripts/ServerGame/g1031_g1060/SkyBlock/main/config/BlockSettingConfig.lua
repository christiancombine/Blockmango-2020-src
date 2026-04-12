
BlockSettingConfig = {}
BlockSettingConfig.BlockSeting = {}

BlockSettingConfig.NOT_DROP = 0
BlockSettingConfig.DROP = 1

function BlockSettingConfig:initConfig(BlockSeting)
    for _, config in pairs(BlockSeting) do
        local curBlock = {}
        curBlock.BlockId = tonumber(config.BlockId) or 0
        curBlock.Meta = tonumber(config.Meta) or 0
        curBlock.Hardness = tonumber(config.Hardness) or 0

        curBlock.DefaultHurt = tonumber(config.DefaultHurt) or 0
        curBlock.DefaultDrop = tonumber(config.DefaultDrop) or 0
        curBlock.Integral = tonumber(config.Integral) or 0

        local DropItemId = StringUtil.split(tostring(config.DropItemId), "#") or ""
        local DropItemMeta = StringUtil.split(tostring(config.DropItemMeta), "#") or ""
        local DropProbability = StringUtil.split(tostring(config.DropProbability), "#") or ""
        local DropCount = StringUtil.split(tostring(config.DropCount), "#") or ""

        local HoldItemId = StringUtil.split(tostring(config.HoldItemId), "#") or ""
        local HoldItemHurt = StringUtil.split(tostring(config.HoldItemHurt), "#") or ""
        local DropItem = StringUtil.split(tostring(config.DropItem), "#") or ""

        curBlock.DropData = {}
        curBlock.HoldItemData = {}

        for k, v in pairs(DropItemId) do
            local data = {
                ItemId = tonumber(DropItemId[k]),
                Meta = tonumber(DropItemMeta[k]),
                Probability = tonumber(DropProbability[k]),
                Count = tonumber(DropCount[k])
            }
            table.insert(curBlock.DropData, data)
        end

        for k, v in pairs(HoldItemId) do
            local data = {
                ItemId= tonumber(HoldItemId[k]) or tonumber(HoldItemId[1]),
                Hurt = tonumber(HoldItemHurt[k]) or tonumber(HoldItemHurt[1]),
                DropBlock = tonumber(DropItem[k]) or tonumber(DropItem[1])
            }
            table.insert(curBlock.HoldItemData, data)
        end

        table.insert(self.BlockSeting, curBlock)
    end
end

function BlockSettingConfig:getBlockSetingByBlockIdAndMeta(blockId, meta)
    for k, block in pairs(self.BlockSeting) do
        if block.BlockId == blockId then
            if blockId == BlockID.LEAVES then
                local cur_meta = meta % 4
                if (block.Meta == cur_meta) then
                    return block
                end
            else
                if self:isMetaUncertain(blockId) or (block.Meta == -1 or block.Meta == meta) then
                    return block
                end
            end
        end
    end

    return nil
end

function BlockSettingConfig:isMetaUncertain(blockId)
    return blockId == BlockID.TORCH_WOOD
        or blockId == BlockID.CHEST
        or blockId == BlockID.LADDER
        or blockId == BlockID.FENCE_GATE
        or blockId == BlockID.WHITE_FENCE_GATE
        or blockId == BlockID.FURNACE_BURNING
        or blockId == BlockID.FURNACE_IDLE
        or blockId == BlockID.DOOR_WOOD
        or blockId == BlockID.WHITE_WOOD_DOOR
        or blockId == BlockID.SIGN_POST
        or blockId == BlockID.SIGN_WALL
end

function BlockSettingConfig:prepareBlockHardness()
    for k, block in pairs(self.BlockSeting) do
        HostApi.setBlockAttr(block.BlockId, block.Hardness)
    end
end

function BlockSettingConfig:getIntegralByBlockId(blockId, blockMeta)
    for i, k in pairs(self.BlockSeting) do
        if k.BlockId == blockId and k.Meta == blockMeta then
            return k.Integral
        end
    end
    return 0
end


function BlockSettingConfig:BreakBlockDropItem(player, blockId, blockMeta, vec3)
    local config = BlockSettingConfig:getBlockSetingByBlockIdAndMeta(blockId, blockMeta)
    if config == nil then
        return
    end

    local isDrop = config.DefaultDrop
    for k, value in pairs(config.HoldItemData) do
        if value.ItemId == player.inHandItemId then
            isDrop = value.DropBlock
        end
    end

    if isDrop == BlockSettingConfig.DROP then
        local pos = VectorUtil.toVector3(vec3)
        local motion = VectorUtil.newVector3(0, 0, 0)
        for k, value in pairs(config.DropData) do
            local num = 0
            for i = 1, value.Count do
                local Probability = HostApi.random("DropItem", 1, 100)
                if Probability <= value.Probability then
                    num = num + 1
                end
            end

            if num > 0 then
                if value.ItemId == BlockID.COBBLESTONE then
                    pos.y = pos.y + 0.5
                end
                EngineWorld:addEntityItem(value.ItemId, num, value.Meta, 600, pos, motion, false, true)
            end
        end
    end
end


return BlockSettingConfig