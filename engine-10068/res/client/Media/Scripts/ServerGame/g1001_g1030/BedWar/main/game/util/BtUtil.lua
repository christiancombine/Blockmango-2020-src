BtUtil = {}

---等同空气方块Map
local AirMap = {
    [tostring(BlockID.AIR)] = true, ---0
    [tostring(BlockID.TALL_GRASS)] = true, ---31  草
    --[tostring(BlockID.WOOD_SINGLE_SLAB)] = true, ---126 木半砖
}
---记录不能站的方块id
local NotStandMap = {
    [tostring(BlockID.WATERLILY)] = true, ---111
    [tostring(BlockID.FENCE)] = true, ---85
    [tostring(BlockID.WATER_MOVING)] = true, ---8
    [tostring(BlockID.WATER_STILL)] = true, ---10
    [tostring(BlockID.PLANT_RED)] = true, ---38   花
    [tostring(BlockID.PLANT_YELLOW)] = true, ---37   花
    [tostring(BlockID.COBBLESTONE_WALL)] = true, ---139
    [tostring(BlockID.STAIND_GLASS_PANE)] = true, ---160
}

local canBreakMap = {
    [tostring(BlockID.CLOTH)] = true, ---35 羊毛
    [tostring(BlockID.WHITE_STONE)] = true, ---121  石头
    [tostring(BlockID.STAIND_GLASS)] = true, ---95 染色玻璃
    [tostring(BlockID.GLASS)] = true, ---20  玻璃
    [tostring(BlockID.PLANKS)] = true, ---5  木板
    [tostring(BlockID.OBSIDIAN)] = true, ---49 黑曜石
    [tostring(BlockID.SLIME)] = true, ---165 粘液块
}

function BtUtil.canBreakBlock(blockId)
    if blockId > 256 then
        return true ---自定义方块可以破坏
    end
    return canBreakMap[tostring(blockId)] or false
end

local pathArray = {}
local BlockIdMap = {}

--计算两点间距离
function BtUtil.calculateDistance(pos1, pos2)
    return math.sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y) + (pos1.z - pos2.z) * (pos1.z - pos2.z))
end

local function getBlockId(pos)
    local blockId
    if BlockIdMap[VectorUtil.hashVector3(pos)] == nil then
        blockId = EngineWorld:getBlockId(pos)
        BlockIdMap[VectorUtil.hashVector3(pos)] = blockId
    else
        blockId = BlockIdMap[VectorUtil.hashVector3(pos)]
    end
    return blockId
end

--将路线进行分段并返回第一段的端点坐标
local function getStagePath(starPos, endPos)
    local StagePoint
    local distance = BtUtil.calculateDistance(starPos, endPos)
    local vec3 = VectorUtil.sub3(endPos, starPos)
    local vecPos = VectorUtil.newVector3(vec3.x / (distance / 5), vec3.y, vec3.z / (distance / 5))
    StagePoint = VectorUtil.add3(starPos, vecPos)
    StagePoint = VectorUtil.toVector3i(StagePoint)

    while getBlockId(StagePoint) ~= 0 do
        if vec3.x >= 0 then
            StagePoint.x = StagePoint.x + 1
        else
            StagePoint.x = StagePoint.x - 1
        end
        if vec3.z >= 0 then
            StagePoint.z = StagePoint.z + 1
        else
            StagePoint.z = StagePoint.z - 1
        end
    end

    return StagePoint
end


local pathFinder = PathPlannerManager.Instance():newPathPlanner('BedwarPathPlanner')

local function startFindPath2(startPos, endPos)
    pathArray = {}
    pathFinder:findPath(startPos, endPos, 100)
    local size = pathFinder:getPathSize()
    if size == 1 then
        return
    end
    for i = size - 1, 0, -1 do
        local pos = pathFinder:getPosInPath(i)
        table.insert(pathArray, pos)
    end
end

--获取当前路径
function BtUtil.getCurPath(starPos, endPos)
    ---如果距离大于10,进行分段
    if BtUtil.calculateDistance(starPos, endPos) > 6 then
        endPos = getStagePath(starPos, endPos)
    end
    startFindPath2(starPos, endPos)
    return pathArray
end

function BtUtil.checkBlockCanMove(pos)
    local blockId = getBlockId(pos)
    ---检查方块能否移动（破坏），如果可以返回true
    return AirMap[tostring(blockId)] or (not BtUtil.isScenesBlock(pos, blockId) and (blockId == BlockID.AIR or blockId == BlockID.TALL_GRASS or blockId == BlockID.WATER_MOVING
            or blockId == BlockID.WATER_STILL or blockId == BlockID.WATERLILY or blockId == BlockID.BED
            --or blockId == BlockID.WOOD_SINGLE_SLAB or blockId == BlockID.STONE_SINGLE_SLAB
            or BtUtil.canBreakBlock(blockId)))
end

function BtUtil.isScenesBlock(blockPos, blockId)
    ---检查特定方块是否是地图方块，如果不是返回false
    if blockId ~= BlockID.AIR and blockId ~= BlockID.BED then
        return not EngineWorld:isBlockChange(blockPos)
    end
    return false
end