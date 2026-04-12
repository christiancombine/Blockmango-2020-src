---
--- Created by Jimmy.
--- DateTime: 2018/8/7 0007 11:36
---
SegmentConfig = {}
SegmentConfig.segments = {}

function SegmentConfig:init(segments)
    self:initSegments(segments)
end

function SegmentConfig:initSegments(segments)
    for _, segment in pairs(segments) do
        local item = {}
        item.id = segment.id
        item.level = tonumber(segment.level)
        item.name = segment.name
        item.image = segment.image
        item.desc = segment.desc
        item.color = segment.color
        item.title = segment.title
        local range = StringUtil.split(segment.pkscore, "#")
        item.pkscore = { min = tonumber(range[1]), max = tonumber(range[2]) }
        self.segments[tonumber(item.id)] = item
    end
end

function SegmentConfig:getSegmentByScore(pkscore)
    local result
    for _, segment in pairs(self.segments) do
        if result == nil then
            result = segment
        end
        if pkscore >= segment.pkscore.min and pkscore <= segment.pkscore.max then
            result = segment
            break
        end
    end
    return result
end

return SegmentConfig