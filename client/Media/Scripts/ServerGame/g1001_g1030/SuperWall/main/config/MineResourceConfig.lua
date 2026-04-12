---
--- Created by Jimmy.
--- DateTime: 2018/8/24 0024 10:36
---
require "data.GameMineResource"

MineResourceConfig = {}
MineResourceConfig.mines = {}

function MineResourceConfig:init(mines)
    self:initMines(mines)
end

function MineResourceConfig:initMines(mines)
    for _, mine in pairs(mines) do
        local item = {}
        item.id = mine.id
        item.blockId = tonumber(mine.blockId)
        item.startPos = VectorUtil.newVector3i(mine.x1, mine.y1, mine.z1)
        item.endPos = VectorUtil.newVector3i(mine.x2, mine.y2, mine.z2)
        item.effect = mine.effect
        item.yaw = tonumber(mine.yaw)
        item.itemId = tonumber(mine.itemId)
        item.num = tonumber(mine.num)
        item.damage = tonumber(mine.damage)
        item.score=tonumber(mine.score)
        self.mines[#self.mines + 1] = item
    end
end

function MineResourceConfig:prepareMines()
    for _, mine in pairs(self.mines) do
        GameMineResource.new(mine)
    end
end

return MineResourceConfig