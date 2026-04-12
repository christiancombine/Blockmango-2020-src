
Area = class()

Area.minX = 0
Area.maxX = 0
Area.minZ = 0
Area.maxZ = 0
Area.minY = 0
Area.maxY = 0

function Area:ctor(area)
    self:setMatrix(area)
end

function Area:setMatrix(matrix)
    self.minX = tonumber(matrix[1][1])
    self.maxX = tonumber(matrix[1][1])
    self.minY = tonumber(matrix[1][2])
    self.maxY = tonumber(matrix[1][2])
    self.minZ = tonumber(matrix[1][3])
    self.maxZ = tonumber(matrix[1][3])
    for i, v in pairs(matrix) do
        if tonumber(v[1]) < self.minX then
            self.minX = tonumber(v[1])
        end
        if tonumber(v[1]) > self.maxX then
            self.maxX = tonumber(v[1])
        end
        if tonumber(v[2]) < self.minY then
            self.minY = tonumber(v[2])
        end
        if tonumber(v[2]) > self.maxY then
            self.maxY = tonumber(v[2])
        end
        if tonumber(v[3]) < self.minZ then
            self.minZ = tonumber(v[3])
        end
        if tonumber(v[3]) > self.maxZ then
            self.maxZ = tonumber(v[3])
        end
    end
end

function Area:inArea(vec3)
    return vec3.x >= self.minX and vec3.x <= self.maxX
    and vec3.y >= self.minY and vec3.y <= self.maxY
    and vec3.z >= self.minZ and vec3.z <= self.maxZ
end

return Area