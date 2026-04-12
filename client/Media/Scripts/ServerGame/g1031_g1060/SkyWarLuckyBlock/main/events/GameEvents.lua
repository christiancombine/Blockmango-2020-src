PlayerSelectJobEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("SelectJobId", PlayerSelectJobEvent)

--刷新职业奖池事件
PlayerRefreshJobListInfoEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("RefreshJobListInfo", PlayerRefreshJobListInfoEvent)


--从奖池里随机职业的给玩家  ExtractRandomJobId
--玩家抽取职业的事件
PlayerExtractJobEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("ExtractRandomJobId", PlayerExtractJobEvent)

PlayerAbandonItemEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("PlayerAbandonItem", PlayerAbandonItemEvent)

PlayerBackHallEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BackHallEvent", PlayerBackHallEvent)

PlayerNextGameEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("NextGameEvent", PlayerNextGameEvent)