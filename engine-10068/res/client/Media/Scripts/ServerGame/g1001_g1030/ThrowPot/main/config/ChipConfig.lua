---
--- Created by Jimmy.
--- DateTime: 2018/7/18 0018 17:35
---
ChipConfig = {}
ChipConfig.chips = {}

function ChipConfig:init(chips)
    self:initChips(chips)
end

function ChipConfig:initChips(chips)
    for id, chip in pairs(chips) do
        local item = {}
        item.id = chip.id
        item.itemId = tonumber(chip.itemId)
        item.level = tonumber(chip.level)
        item.tip = tonumber(chip.tip)
        self.chips[tonumber(id)] = item
    end
end

function ChipConfig:getChip(id)
    return self.chips[tonumber(id)]
end

function ChipConfig:getChipByItemId(itemId)
    for id, chip in pairs(self.chips) do
        if chip.itemId == itemId then
            return chip
        end
    end
    return nil
end

function ChipConfig:randomChipByLevel(level)
    local chips = {}
    for id, chip in pairs(self.chips) do
        if level == chip.level then
            chips[#chips + 1] = chip
        end
    end
    if #chips > 0 then
        return chips[math.random(1, #chips)]
    else
        return self.chips[1]
    end
end

function ChipConfig:randomChip(random)
    local chip
    if random <= 2 then
        chip = ChipConfig:randomChipByLevel(3)
    elseif random <= 14 then
        chip = ChipConfig:randomChipByLevel(2)
    else
        chip = ChipConfig:randomChipByLevel(1)
    end
    return chip
end

return ChipConfig