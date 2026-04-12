
---
--- Created by Jimmy.
--- DateTime: 2018/3/3 0003 14:47
---

HotSpringConfig = {}
HotSpringConfig.data = {}

function HotSpringConfig:init(config)
    self:initData(config)
end

function HotSpringConfig:initData(config)
    for i, v in pairs(config) do
        if tonumber(GameConfig.mapId) == tonumber(v.mapId) then
            local item = {}
            item.teamId = tonumber(v.teamId)
            item.hpSchemetic = tostring(v.hpSchemetic)

            local hpSchemeticPos = StringUtil.split(tostring(v.hpSchemeticPos), ",")
            item.hpSchemeticPos = VectorUtil.newVector3(hpSchemeticPos[1], hpSchemeticPos[2], hpSchemeticPos[3])

            local hpStart = StringUtil.split(tostring(v.hpStart), ",")
            item.hpStart = VectorUtil.newVector3i(hpStart[1], hpStart[2], hpStart[3])

            local hpEnd = StringUtil.split(tostring(v.hpEnd), ",")
            item.hpEnd = VectorUtil.newVector3i(hpEnd[1], hpEnd[2], hpEnd[3])

            self.data[item.teamId] = item
        end
    end

    table.sort( self.data, function ( a, b )
        return a.teamId < b.teamId
    end )
end

function HotSpringConfig:getSchemeticPos(team)
    if team ~= nil and team ~= 0 then
        return self.data[team].hpSchemeticPos
    end
    return VectorUtil.ZERO
end

function HotSpringConfig:getSchemetic(team)
    if team ~= nil and team ~= 0 then
        return self.data[team].hpSchemetic
    end
    return nil
end

function HotSpringConfig:getStartPos(team)
    if team ~= nil and team ~= 0 then
        return self.data[team].hpStart
    end
    return VectorUtil.ZERO
end

function HotSpringConfig:getEndPos(team)
    if team ~= nil and team ~= 0 then
        return self.data[team].hpEnd
    end
    return VectorUtil.ZERO
end


return HotSpringConfig