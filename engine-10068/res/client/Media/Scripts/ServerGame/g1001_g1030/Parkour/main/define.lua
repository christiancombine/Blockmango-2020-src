
local define = {}


define.gameType = "g1021"

define.MATCH_STAGE_NONE     = 0     -- ��ʼ״̬
define.MATCH_STAGE_WAIT     = 1     -- �ȴ����
define.MATCH_STAGE_READY    = 2     -- ׼���׶�
define.MATCH_STAGE_START    = 3     -- �����ڼ�
define.MATCH_STAGE_CLOSE    = 4     -- ��������

define.RANK_WEEK_FLAG       = string.format("%s.week", define.gameType)
define.RANK_HIST_FLAG       = string.format("%s.hist", define.gameType)

define.PLAYER_DB_CHUNK_DEFAULT = 1

return define
