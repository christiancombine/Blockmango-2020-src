---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---

require "config.TreeSectionConfig"
require "config.BlockSettingConfig"
require "config.ToolDurableConfig"
require "config.IslandLevelConfig"
require "config.IslandAreaConfig"
require "config.TaskConfig"
require "config.RewardConfig"
require "config.BoneMealConfig"
require "config.CropsConfig"
require "config.SkillConfig"
require "config.Define"
require "config.AppShopConfig"
require "config.SellItemConfig"
require "config.ProductConfig"
require "config.RankConfig"
require "config.PrivilegeConfig"
require "config.GunConfig"
require "config.FuncNpcConfig"
require "config.SignInConfig"
require "config.PartyConfig"
require "config.GemStoneConfig"
require "config.ConfigurableBlockConfig"
require "config.ChristmasConfig"
require "config.GiftRewardConfig"
require "config.ChristmasNpcConfig"
require "config.CustomItemConfig"

GameConfig = {}
GameConfig.RootPath = ""

function GameConfig:init(config)
    self.initHP = tonumber(config.initHP or 20)
    self.hungerValue = tonumber(config.hungerSubHP[1] or 1)
    self.hungerInterval = tonumber(config.hungerSubHP[2] or 2)
    self.maxFoodAddHp = config.maxFoodAddHp
    self.deathLineY = tonumber(config.deathLineY or 2)
    self.dirtTime = tonumber(config.dirtTime or 20)
    self.maxBook = tonumber(config.maxBook or 8)
    self.lavaDamageCoe = tonumber(config.lavaDamageCoe or 0.15)
    self.lavaFireTime = tonumber(config.lavaFireTime or 6)
    self.onFireDamage = tonumber(config.onFireDamage or 1)
    self.manorMax = tonumber(config.manorMax) or 100
    self.manorChunk = tonumber(config.manorChunk) or 20
    self.manorStart = config.manorStart
    self.manorNear = tonumber(config.manorNear) or 3
    self.boxLimit = tonumber(config.boxLimit)
    self.boxInit = config.boxInit
    self.furnaceTime = tonumber(config.furnaceTime)
    self.furnaceLimit = tonumber(config.furnaceLimit)
    self.signPostNum = tonumber(config.signPostNum) or 50
    self.canPlaceActorLimit = tonumber(config.canPlaceActorLimit) or 10
    self.canPlaceItem = config.canPlaceItem
    self.refreshDareTask = config.refreshDareTask[1]
    self.buyDaretask = config.buyDaretask[1]
    self.initSchemetic = tostring(config.initSchemetic)
    self.initSchematicOffset = tonumber(config.initSchematicOffset)
    self.homeCD = tonumber(config.homeCD or 5)
    self.hallCD = tonumber(config.hallCD or 5)
    self.miningAreaCD = tonumber(config.miningAreaCD or 5)
    self.productCD = tonumber(config.productCD or 5)
    self.invinciblyCD = tonumber(config.invinciblyCD or 5)
    self.moonName = tostring(config.moonName) or "moon_phases.png"
    self.sunName = tostring(config.sunName) or "sun.png"
    self.ranks = config.ranks
    self.OtherSchmetic = config.OtherSchmetic
    self.airWall_MaxY = tonumber(config.airWall_MaxY or 254)
    self.monetaryUnit = tonumber(config.monetaryUnit or 200)
    self.genislandPos = VectorUtil.newVector3(config.genislandPosYaw[1] or 0 , config.genislandPosYaw[2] or 300, config.genislandPosYaw[3] or 0)
    self.genislandYaw = tonumber(config.genislandPosYaw[4] or 0)
    self.genislandLevel = tonumber(config.genislandLevel or 4)
    self.giveLoveNumLevel = tonumber(config.giveLoveNumLevel) or 1
    self.partyMaxPlayer = tonumber(config.partyMaxPlayer) or 16
    self.welcomeText = config.welcomeText or ""
    self.signInStartTime = config.signInStartTime or "0"
    self.signInEndTime = config.signInEndTime or "0"
    self.createPartySuccessTip = config.createPartySuccessTip or ""
    self.onceBuyMoneyCost = tonumber(config.onceBuyMoneyCost or 60)
    self.maxBuyMoneyCost = tonumber(config.maxBuyMoneyCost or 600)
    self.onceMoneyNum = tonumber(config.onceMoneyNum or 100000)
    self.waitTime = config.waitTime or 28800
    self.christmasFinishTime = config.christmasFinishTime or 1578499200
    self.snowTime = config.snowTime

    RankConfig:init(self.ranks)
    IslandAreaConfig:init(FileUtil.getConfigFromCsv("IslandAreaConfig.csv"))
    IslandLevelConfig:init(FileUtil.getConfigFromCsv("IslandLevelConfig.csv"))
    TreeSectionConfig:init(FileUtil.getConfigFromCsv("TreeSectionConfig.csv"))
    BoneMealConfig:init(FileUtil.getConfigFromCsv("BoneMealConfig.csv"))
    CropsConfig:init(FileUtil.getConfigFromCsv("CropsConfig.csv"))
    SkillConfig:init(FileUtil.getConfigFromCsv("SkillConfig.csv"))
    ProductConfig:init(FileUtil.getConfigFromCsv("ProductConfig.csv"))
    SellItemConfig:init(FileUtil.getConfigFromCsv("setting/SellItem.csv", 3))
    BlockSettingConfig:initConfig(FileUtil.getConfigFromCsv("setting/BlockSetting.csv", 3))
    ToolDurableConfig:initConfig(FileUtil.getConfigFromCsv("ToolDurable.csv"))
    TaskConfig:initConfig(FileUtil.getConfigFromCsv("setting/SkyBlockTaskSetting.csv", 3))
    TaskConfig:initPossibleConfig(FileUtil.getConfigFromCsv("TaskPossible.csv", 3))
    RewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/SkyBlockTaskRewardSetting.csv", 3))
    AppShopConfig:initConfig(FileUtil.getConfigFromCsv("setting/AppShopSetting.csv", 3))
    AppShopConfig:initSortConfig(FileUtil.getConfigFromCsv("setting/AppShopSortSetting.csv", 3))
    PrivilegeConfig:initPrivilege(FileUtil.getConfigFromCsv("PrivilegeConfig.csv"))
    PrivilegeConfig:initItem(FileUtil.getConfigFromCsv("PrivilegeItemConfig.csv"))
    GunConfig:init(FileUtil.getConfigFromCsv("GunConfig.csv"))
    FuncNpcConfig:initConfig(FileUtil.getConfigFromCsv("NpcConfig.csv"))
    SignInConfig:initConfig(FileUtil.getConfigFromCsv("setting/SkyBlockSignInSetting.csv", 3))
    PartyConfig:initConfig(FileUtil.getConfigFromCsv("setting/PartySetting.csv"))
    ConfigurableBlockConfig:initConfig(FileUtil.getConfigFromCsv("setting/configurableBlock.csv", 3))
    ChristmasConfig:initConfig(FileUtil.getConfigFromCsv("setting/ChristmasSetting.csv"))
    GiftRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/GiftRewardSetting.csv"))
    ChristmasNpcConfig:init(FileUtil.getConfigFromCsv("ChristmasNpcConfig.csv"))
    CustomItemConfig:initConfig(FileUtil.getConfigFromCsv("setting/customItem.csv", 3))
end

function GameConfig:getInitPosByManorIndex(index)
    for _, manor in pairs(self.manorStart) do
        if tonumber(manor.index) == tonumber(index) then
            return VectorUtil.newVector3i(manor.init_pos[1], manor.init_pos[2], manor.init_pos[3])
        end
    end
    return nil
end

function GameConfig:getOriginPosByManorIndex(index)
    for _, manor in pairs(self.manorStart) do
        if tonumber(manor.index) == tonumber(index) then
            return VectorUtil.newVector3i(manor.origin_pos[1], manor.origin_pos[2], manor.origin_pos[3])
        end
    end
    return nil
end

function GameConfig:getChunkMax()
    return math.floor(self.manorMax / self.manorChunk)
end

function GameConfig:checkChunk(chunkIndex)
    local result = chunkIndex
    if result < 0 then
        result = 0
    end
    if result >= self:getChunkMax() then
        result = self:getChunkMax() - 1
    end
    return result
end

function GameConfig:checkManorIndexLimit(index)
    if index < 0 then
        return false
    end

    if index >= math.ceil(self.manorMax / self.manorStart) - 1 then
        return false
    end
end

function GameConfig:getInitBox(vec3)
    if self.boxInit then
        for k, v in pairs(self.boxInit) do
            if vec3.x == tonumber(v.pos[1]) and vec3.y == tonumber(v.pos[2]) and tonumber(v.pos[3]) then
                return v.items
            end
        end
    end

    return nil
end

function GameConfig:isInitBoxPos(x, y, z)
    if self.boxInit then
        for k, v in pairs(self.boxInit) do
            if x == tonumber(v.pos[1]) and y == tonumber(v.pos[2]) and z == tonumber(v.pos[3]) then
                return true
            end
        end
    end

    return false
end

function GameConfig:decodeHash(hash)
    local hashString = StringUtil.split(hash, ":")
    if hashString then
        return tonumber(hashString[1]), tonumber(hashString[2]), tonumber(hashString[3])
    end
    return 0, 0, 0
end

function GameConfig:decodeValue(value)
    local valueString = StringUtil.split(value, ":")
    if valueString then
        return tonumber(valueString[1]), tonumber(valueString[2])
    end
    return 0, 0
end

function GameConfig:isDoorItemBlock(id)
    return id == 324 or id == 330 or id == 510
end

function GameConfig:isDoorBlock(id)
    return id == BlockID.WHITE_WOOD_DOOR
    or id == BlockID.DOOR_WOOD
    or id == BlockID.DOOR_IRON
    or id == BlockID.FENCE_GATE
    or id == BlockID.WHITE_FENCE_GATE
end

function GameConfig:isHoe(id)
    return id == ItemID.HOE_WOOD
    or id == ItemID.HOE_WOOD
    or id == ItemID.HOE_STONE
    or id == ItemID.HOE_IRON
    or id == ItemID.HOE_DIAMOND
    or id == ItemID.HOE_GOLD or id == ItemID.HOE_PURPLE
end

function GameConfig:isTool(id)
    return id == ItemID.HOE_WOOD
     or id == ItemID.HOE_PURPLE
     or id == ItemID.HOE_STONE
     or id == ItemID.HOE_IRON
     or id == ItemID.HOE_DIAMOND
     or id == ItemID.HOE_GOLD
     or id == ItemID.SHOVEL_IRON
     or id == ItemID.SHOVEL_WOOD
     or id == ItemID.SHOVEL_STONE
     or id == ItemID.SHOVEL_DIAMOND
     or id == ItemID.SHOVEL_GOLD
     or id == ItemID.SHOVEL_PURPLE
     or id == ItemID.AXE_IRON
     or id == ItemID.AXE_WOOD
     or id == ItemID.AXE_STONE
     or id == ItemID.AXE_DIAMOND
     or id == ItemID.AXE_GOLD
     or id == ItemID.AXE_PURPLE
     or id == ItemID.PICKAXE_IRON
     or id == ItemID.PICKAXE_WOOD
     or id == ItemID.PICKAXE_STONE
     or id == ItemID.PICKAXE_DIAMOND
     or id == ItemID.PICK_AXE_GOLD
     or id == ItemID.PICKAXE_PURPLE
     or id == ItemID.ARROW
     or id == BlockID.SIGN_POST
     or id == BlockID.SIGN_WALL
end

function GameConfig:isAttachmentBlock(id)
    return id == BlockID.LADDER
     or id == BlockID.WOOL_CARPET
     or id == BlockID.TORCH_WOOD
     or id == BlockID.ALLIUM
     or id == BlockID.BLUE_ORCHID
     or id == BlockID.HOUSTONIA
     or id == BlockID.OXEYE_DAISY
     or id == BlockID.PAEONIA
     or id == BlockID.TULIP_PINK
     or id == BlockID.TULIP_RED
     or id == BlockID.TULIP_WHITE
     or id == BlockID.SAPLING
     or id == BlockID.SIGN_POST
     or id == BlockID.SIGN_WALL
     or id == BlockID.SAPLING

end

function GameConfig:isStairsBlock(id)
    return id == BlockID.STAIRS_WOOD_OAK
     or id == BlockID.STAIRS_COBBLESTONE
     or id == BlockID.STAIRS_BRICK
     or id == BlockID.STAIRS_STONE_BRICK
     or id == BlockID.STAIRS_NETHER_BRICK
     or id == BlockID.STAIRS_SAND_STONE
     or id == BlockID.STAIRS_WOOD_SPRUCE
     or id == BlockID.STAIRS_WOOD_BIRCH
     or id == BlockID.STAIRS_WOOD_JUNGLE
     or id == BlockID.STAIRS_NETHER_QUARTZ
     or id == BlockID.STAIRS_WOOD_DARKOAK
     or id == BlockID.BLOCK_ID_STAIRS_PRISMARINE
     or id == BlockID.BLOCK_ID_STAIRS_PRISMARINE_BRICK
     or id == BlockID.BLOCK_ID_STAIRS_DARK_PRISMARINE
     or id == BlockID.BLOCK_ID_STAIRS_RED_NETHER_BRICK
     or ConfigurableBlockConfig:isStairs(id)
end

function GameConfig:checkPosHasBlock(id, aPos)
    local nowId = EngineWorld:getBlockId(aPos)
    if nowId == id then
        return true
    end
    return false
end

function GameConfig:isCommonHalfSlab(id)
    return id == BlockID.STONE_SINGLE_SLAB
            or id == BlockID.WOOD_SINGLE_SLAB
end

function GameConfig:isMoreThan1000HalfSlabItem(id)
    return id == BlockID.BLOCK_ID_PRISMARINE_SINGLE_SLAB + 1000
    or ConfigurableBlockConfig:isSingleHalfSlab(id - 1000)
end

function GameConfig:isMoreThan1000HalfSlabBlock(id)
    return id == BlockID.BLOCK_ID_PRISMARINE_SINGLE_SLAB
    or ConfigurableBlockConfig:isSingleHalfSlab(id)
end

function GameConfig:metaHandleFurnace(yaw, meta)
    local new_meta = meta
    local new_yaw = math.floor(yaw)
    while new_yaw < 0 do
        new_yaw = new_yaw + 360
    end

    while new_yaw > 360 do
        new_yaw = new_yaw - 360
    end

    if new_yaw >= 45 and new_yaw < 135 then
        new_meta = 5
    elseif new_yaw >= 135 and new_yaw < 225 then
        new_meta = 3
    elseif new_yaw >= 225 and new_yaw < 315 then
        new_meta = 4
    else
        new_meta = 2
    end
    return new_meta
end

function GameConfig:metaHandleGate(yaw, meta)
    local new_meta = meta
    local new_yaw = math.floor(yaw)
    while new_yaw < 0 do
        new_yaw = new_yaw + 360
    end

    while new_yaw > 360 do
        new_yaw = new_yaw - 360
    end

    if new_yaw >= 45 and new_yaw < 135 then
        new_meta = 5
    elseif new_yaw >= 135 and new_yaw < 225 then
        new_meta = 0
    elseif new_yaw >= 225 and new_yaw < 315 then
        new_meta = 1
    else
        new_meta = 2
    end
    return new_meta
end

function GameConfig:metaHandleStairs(yaw, meta)
    local new_meta = meta

    local new_yaw = math.floor(yaw)
    while new_yaw < 0 do
        new_yaw = new_yaw + 360
    end

    while new_yaw > 360 do
        new_yaw = new_yaw - 360
    end

    if meta == 4 then
        if new_yaw >= 45 and new_yaw < 135 then
            new_meta = 5
        elseif new_yaw >= 135 and new_yaw < 225 then
            new_meta = 7
        elseif new_yaw >= 225 and new_yaw < 315 then
            new_meta = 4
        else
            new_meta = 6
        end
    else
        if new_yaw >= 45 and new_yaw < 135 then
            new_meta = 1
        elseif new_yaw >= 135 and new_yaw < 225 then
            new_meta = 3
        elseif new_yaw >= 225 and new_yaw < 315 then
            new_meta = 0
        else
            new_meta = 2
        end
    end

    return new_meta
end

function GameConfig:isMoreThan1000Block(id)
    return id == BlockID.BLOCK_ID_PRISMARINE + 1000
    or id == BlockID.BLOCK_ID_PRISMARINE_BRICK + 1000
    or id == BlockID.BLOCK_ID_DARK_PRISMARINE + 1000
    or id == BlockID.BLOCK_ID_STARRY_SKY + 1000
    or id == BlockID.BLOCK_ID_STAIRS_PRISMARINE + 1000
    or id == BlockID.BLOCK_ID_STAIRS_PRISMARINE_BRICK + 1000
    or id == BlockID.BLOCK_ID_STAIRS_DARK_PRISMARINE + 1000
    or id == BlockID.BLOCK_ID_STAIRS_RED_NETHER_BRICK + 1000
    or id == BlockID.BLOCK_ID_PRISMARINE_DOUBLE_SLAB + 1000
    or id == BlockID.BLOCK_ID_PRISMARINE_SINGLE_SLAB + 1000
    or id == BlockID.BONEMEAL + 1000
    or id == BlockID.ALLIUM + 1000
    or id == BlockID.BLUE_ORCHID + 1000
    or id == BlockID.HOUSTONIA + 1000
    or id == BlockID.OXEYE_DAISY + 1000
    or id == BlockID.PAEONIA + 1000
    or id == BlockID.TULIP_PINK + 1000
    or id == BlockID.TULIP_RED + 1000
    or id == BlockID.TULIP_WHITE + 1000 or ConfigurableBlockConfig:isConfigurableBlock(id - 1000)
end

function GameConfig:checkBlockInArea(vec3, player, targetManorInfo)

    if player == nil then
        return false
    end

    if targetManorInfo == nil then
        return false
    end

    local Vector3i = GameConfig:getOriginPosByManorIndex(targetManorInfo.manorIndex)
    local radius = IslandAreaConfig:getRadiusByArea(player.area)
    print("userId == "..tostring(player.userId).."  targetManorInfo manorIndex == " .. targetManorInfo.manorIndex.."  radius == "..radius
        .."Vector3ix == "..Vector3i.x.." Vector3iy == "..Vector3i.y.. " Vector3iz == "..Vector3i.z)
    return vec3.x >= Vector3i.x - radius and vec3.x <= Vector3i.x + (radius - 1)
            and vec3.y >= Vector3i.y - radius and vec3.y <= Vector3i.y + (radius - 1)
            and vec3.z >= Vector3i.z - radius and vec3.z <= Vector3i.z + (radius - 1)
end

function GameConfig:isCanPlaceItem(id)
    for k,v in pairs(self.canPlaceItem) do
        if tonumber(v.itemId) == tonumber(id)  then
            return true, tostring(v.itemActor)
        end
    end
    return false, ""
end

function GameConfig:getBlockIdWithPlaceItemId(id)
    for _, v in pairs(self.canPlaceItem) do
        if tonumber(v.itemId) == tonumber(id)  then
            return tonumber(v.lightBlock) or -1
        end
    end
    return -1
end

function GameConfig:isInSnowTime(time)
    if self.snowTime then
        for _, info in pairs(self.snowTime) do
            local down_time = tonumber(info.time[1])
            local up_time = tonumber(info.time[2])
            if down_time < up_time then
                if time > down_time and time < up_time then
                    return true
                end
            end
        end
    end
    return false
end

return GameConfig