package.path = package.path .. ';..\\?.lua';

BaseListener.registerCallBack(GameInitEvent, function(config)
    if BaseMain:getGameType() == "g1046" then
        require "hall.BedWarClient"
    else
        require "game.BedWarClient"
    end
    BedWarClient.onGameInit()
	local deviation = 3 -- 辉光的宽度
	local intensity = 2.6 --- 辉光的强度
	local saturation = 1.0 --  辉光颜色对场景的影响 (0-1.0)
	Blockman.Instance():setBlockBloomOption(deviation, intensity, saturation)
    ClientHelper.putStringPrefs("RechargeGameType", "g1008")
end)
