CommonConfig = {}

function CommonConfig:loadConfig()
    local config = FileUtil.getConfigFromCsv("setting/Common.csv", 2)

    --CommonConfig.cameraOffsetScale = tonumber(config["1"].n_cameraOffsetScale) or 0 --摄像机高度
    CommonConfig.renderDistanceWeight = tonumber(config["1"].n_renderDistanceWeight) or 64
    CommonConfig.extraCdTime = tonumber(config["1"].n_extraCdTime) or 0

end

return CommonConfig
