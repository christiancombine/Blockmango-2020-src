---
--- Created by Jimmy.
--- DateTime: 2018/3/6 0006 11:30
---

require "config.Area"

BlockConfig = {}
BlockConfig.block = {}
BlockConfig.blockHp = {}
BlockConfig.area = {}

function BlockConfig:init(config)
    self:initBlock(config.block)
    self:initArea(config.area)
end

function BlockConfig:initBlock(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.meta = tonumber(v.meta or "0")
        item.hardness = tonumber(v.hardness)
        item.hp = tonumber(v.hp)
        self.block[item.id] = item
    end
end

function BlockConfig:initArea(area)
    self.area = Area.new(area)
end

function BlockConfig:prepareBlock()
    for i, v in pairs(self.block) do
        HostApi.setBlockAttr(v.id, v.hardness)
    end
end

return BlockConfig