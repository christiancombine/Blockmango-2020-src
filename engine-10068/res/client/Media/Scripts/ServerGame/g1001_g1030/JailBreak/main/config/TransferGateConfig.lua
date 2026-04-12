require "config.Area"

TransferGateConfig = {}
TransferGateConfig.transferGate = {}

function TransferGateConfig:init(config)
    self:initPos(config.transferGate)
end

function TransferGateConfig:initPos(config)
    for i, v in pairs(config) do
        local gate = {}
        gate.oldMap = tonumber(v.oldMap or "0")
        gate.pos = Area.new(v.pos)
        gate.transferPos = {}
        for j, transferPos in pairs(v.transferPos) do
            local tp = {}
            tp.pos = VectorUtil.newVector3(transferPos.pos[1], transferPos.pos[2], transferPos.pos[3])
            gate.transferPos[j] = tp
        end
        self.transferGate[i] = gate
    end
end

function TransferGateConfig:getTransferPosition(transferPos)
    return transferPos[math.random(1,#transferPos)].pos
end

return TransferGateConfig