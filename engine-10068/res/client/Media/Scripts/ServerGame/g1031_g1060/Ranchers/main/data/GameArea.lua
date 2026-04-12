GameArea = class()

function GameArea:ctor(id, startPos, endPos)
    local minX = math.min(startPos.x, endPos.x)
    local maxX = math.max(startPos.x, endPos.x)
    local minZ = math.min(startPos.z, endPos.z)
    local maxZ = math.max(startPos.z, endPos.z)

    self.id = id
    self.topLine = minX .. "," .. minZ .. ":" .. maxX .. "," .. minZ
    self.downLine = minX .. "," .. maxZ .. ":" .. maxX .. "," .. maxZ
    self.leftLine = minX .. "," .. minZ .. ":" .. minX .. "," .. maxZ
    self.rightLine = maxX .. "," .. minZ .. ":" .. maxX .. "," .. maxZ
end

