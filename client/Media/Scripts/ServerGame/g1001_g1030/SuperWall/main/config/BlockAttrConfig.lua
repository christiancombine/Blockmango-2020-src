---
--- Created by Jimmy.
--- DateTime: 2018/3/6 0006 11:30
---

BlockAttrConfig = {}

function BlockAttrConfig:init(blockAttrs)
    self:initBlockAttrs(blockAttrs)
end

function BlockAttrConfig:initBlockAttrs(blockAttrs)
    for _, blockAttr in pairs(blockAttrs) do
        HostApi.setBlockAttr(tonumber(blockAttr.blockId), tonumber(blockAttr.hardness))
    end
end

return BlockAttrConfig