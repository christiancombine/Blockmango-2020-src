---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.MapAreaConfig"

GameConfig = {}

function GameConfig:init(config)
    MapAreaConfig:initMapArea(FileUtil.getConfigFromCsv("MapArea.csv"))
end

return GameConfig