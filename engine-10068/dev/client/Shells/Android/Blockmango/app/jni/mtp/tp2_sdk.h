/*
 * tss_sdk.h
 *
 *  Created on: 2013-3-14
 *      Author: gavin gu
 */

#ifndef TSS_SDK_H
#define TSS_SDK_H

#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)

/* anti数据信息 */
/* anti data info*/
/* 注意:此结构体请勿做任何更改 */
struct TssSdkAntiDataInfo
{
    unsigned short anti_data_len_;   /* [in] length of anti data */
    const unsigned char *anti_data_; /* [in] anti data buffer */
};

struct TssSecSignature
{
	const char *name_;
	const unsigned char *buf_;
	size_t buf_len_;
	uint32_t crc_;
};

/* 发送Anti数据，需由游戏实现 */
/* send anti data, need to be implement by game */
typedef bool (*tss_sdk_send_data_to_svr)(const struct TssSdkAntiDataInfo *anti_data);

struct TssSdkInitInfo
{
	unsigned int size_;                           // struct size
	unsigned int game_id_;                        // game id
	tss_sdk_send_data_to_svr send_data_to_svr_;   // callback interface,implement by game
};

enum TssSdkEntryId
{
	ENTRY_ID_QZONE		= 1,       	// QZone
	ENTRY_ID_MM			= 2,       	// wechat
	ENTRT_ID_FACEBOOK	= 3,		// facebook
	ENTRY_ID_TWITTER	= 4,		// twitter
	ENTRY_ID_LINE		= 5,		// line
	ENTRY_ID_WHATSAPP	= 6,		// whatsapp
	ENTRY_ID_GAMECENTER = 7,		// gamecenter
	ENTRY_ID_GOOGLEPLAY = 8,		// googleplay
	ENTRY_ID_VK = 9,			// vk
	ENTRY_ID_OTHERS		= 99,       // other platform
};

enum TssSdkUinType
{
	UIN_TYPE_INT = 1, // integer format
	UIN_TYPE_STR = 2, // string format
};

enum TssSdkAppIdType
{
    APP_ID_TYPE_INT = 1, // integer format
	APP_ID_TYPE_STR = 2, // string format
};

struct TssSdkUserInfo
{
	unsigned int size_;      // struct size
	unsigned int entry_id_;  // entrance id, wechat/open platform and so on.
	struct
	{
		unsigned int type_;  // type of uin, refer to TssSdkUinType
		union
		{
			unsigned int uin_int_;   // for integer format uin
			char uin_str_[64];       // for string format uin
		};
	}uin_;

    struct
    {
        unsigned int type_;
        union
        {
            unsigned int app_id_int_;   // for integer format appid
			char app_id_str_[64];       // for string format appid
        };
    }app_id_;
};

struct TssSdkUserInfoEx
{
    unsigned int size_;      // struct size
    unsigned int entry_id_;  // entrance id, wechat/open platform and so on.
    struct
    {
        unsigned int type_;  // type of uin, refer to TssSdkUinType
        union
        {
            unsigned int uin_int_;   // for integer format uin
            char uin_str_[64];       // for string format uin
        };
    }uin_;

    struct
    {
        unsigned int type_;
        union
        {
            unsigned int app_id_int_;   // for integer format appid
            char app_id_str_[64];       // for string format appid
        };
    }app_id_;

    unsigned int world_id_;
    char role_id_[64];
};

enum TssSdkGameStatus
{
    GAME_STATUS_FRONTEND = 1,  // runing in front-end
    GAME_STATUS_BACKEND  = 2,  // runing in back-end
    GAME_STATUS_START_PVP = 3,
    GAME_STATUS_END_PVP = 4,
    GAME_STATUS_UPDATE_FINISHED = 5,
};

struct TssSdkGameStatusInfo
{
	unsigned int size_;        // struct size
	unsigned int game_status_; // running in front-end or back-end
};

struct TssSdkEncryptPkgInfo
{
    unsigned int cmd_id_;                     /* [in] game pkg cmd */
    const unsigned char *game_pkg_;           /* [in] game pkg */
    unsigned int game_pkg_len;                /* [in] the length of game data packets, maximum length less than 65,000 */
    unsigned char *encrypt_data_;             /* [in/out] assembling encrypted game data package into anti data, memory allocated by the caller, 64k at the maximum */
    unsigned int encrypt_data_len_;           /* [in/out] length of anti_data when input, actual length of anti_data when output */
};

struct TssSdkDecryptPkgInfo
{
    const unsigned char *encrypt_data_;     /* [in] anti data received by game client */
    unsigned int encrypt_data_len;          /* [in] length of anti data received by game client */
    unsigned char *game_pkg_;               /* [out] buffer used to store the decrypted game package, space allocated by the caller */
    unsigned int game_pkg_len_;             /* [out] input is size of game_pkg_, output is the actual length of decrypted game package */
};

struct TssSetOptsInfo
{
	const char *key;
	bool enable;
	const char *sig;
};

/**
 * TssOpenMTCJTask:
 *
 * 此接口对外开放，请勿做任何修改
 */
struct TssOpenMTCJTask
{
	const char *path;					//要云的文件路径
	uint8_t ignore_flow_limit;			//是否忽略流量控制选项
	uint8_t delete_file_after_upload;	//是否需要在云文件成功后删除文件
	uint32_t unused_1;					//请勿使用
	uint32_t unused_2;					//请勿使用
	uint32_t unused_3;					//请勿使用
	uint32_t unused_4;					//请勿使用
};

/**
 * WinBridgeTask:
 *
 * 此接口对外开放, 请勿做任何修改
 */
struct TssWinBrigeTask
{
	void *session;			//WinBridge Session指针
	const char *cmd;		//命令请求
	const char *response;	//命令答复,请勿释放此成员内存
	uint64_t unused_1;
	uint64_t unused_2;
	uint64_t unused_3;
	uint64_t unused_4;
};


enum TssSdkAntiEncryptResult
{
    ANTI_ENCRYPT_OK = 0,
    ANTI_NOT_NEED_ENCRYPT = 1,
};

enum TssSdkAntiDecryptResult
{
    ANTI_DECRYPT_OK = 0,
    ANTI_DECRYPT_FAIL = 1,
};

#pragma pack(pop)

enum TssInfoType
{
    TSS_INFO_TYPE_DETECT_RESULT = 1,
    TSS_INFO_TYPE_HEARTBEAT = 2,
};

#ifdef __cplusplus
class TssInfoReceiver
{
public:
    virtual ~TssInfoReceiver(){}
    virtual int getInterfaceVersionCode(){return 1;}
    virtual void onReceive(int tssInfoType,const char* info) = 0;
};
#endif

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief game client calls this interface to set initialization information when game starts
 *
 * @param[in] init_info pointing to data of initialization information, defined in TssSdkInitInfo
 *
 */

void __attribute__((visibility("default"))) tss_sdk_init(const struct TssSdkInitInfo* init_info);

/**
 * @brief game client calls this interface to set user information when login in game
 *
 * @param[in] user_info pointing to data of initialization information, defined in TssSdkUserInfo
 *
 * @warning: this interface is expired, call tss_sdk_setuserinfo_ex instead
 *
 */
void __attribute__((visibility("default"))) tss_sdk_setuserinfo(const struct TssSdkUserInfo* user_info);


/**
 * @brief game client calls this interface to set user information when login in game
 *
 * @param[in] user_info pointing to data of initialization information, defined in TssSdkUserInfo
 *
 */
void __attribute__((visibility("default"))) tss_sdk_setuserinfo_ex(const struct TssSdkUserInfoEx* user_info);


/**
 * @brief game client calls this interface to notify the security component game's current status
 *
 * @param[in] game_status if game running in front-end, set it to GAME_STATUS_FRONTEND, if game running in back-end, set it to GAME_STATUS_BACKEND
 */

void __attribute__((visibility("default"))) tss_sdk_setgamestatus(const struct TssSdkGameStatusInfo* game_status);

/**
* @brief Game client calls this interface to encrypt game's packets
*
* @param[in] anti_data Pointing to anti data, defined in TssSdkAntiDataInfo
*
*/
void __attribute__((visibility("default"))) tss_sdk_rcv_anti_data(const struct TssSdkAntiDataInfo *anti_data);

/**
* @brief Game client calls this interface to encrypt game's packets
*
* @param[in] encrypt_pkg_info Pointing to game's packets need encrypting, defined in ENCRYPTPKGINFO
*
* @return refer to the definition of TssSdkAntiEncryptResult.
*/
enum TssSdkAntiEncryptResult __attribute__((visibility("default"))) tss_sdk_encryptpacket(struct TssSdkEncryptPkgInfo *encrypt_pkg_info);

/**
* @brief Game client calls this interface as soon as receives anti packets from game server
*
* @param[in] decrypt_pkg_info Pointing to game's packets need decrypting, defined in DECRYPTPKGINFO
*
* @return refer to the definition of TssSdkAntiDecryptResult
*/
enum TssSdkAntiDecryptResult __attribute__((visibility("default"))) tss_sdk_decryptpacket(struct TssSdkDecryptPkgInfo *decrypt_pkg_info);

/**
* @brief Game client calls this interface as soon as receives game packets from game server
*
* @param[in] cmd_id Pointing to game's packets cmd
*
* @return If return 0, game client does nothing, if return 1, game client needs to discard this packet.
*/
bool __attribute__((visibility("default"))) tss_sdk_ischeatpacket(const unsigned int cmd_id);

/**
 * @brief Game client calls this interface to retrieve a TssSdkAntiDataInfo struct data 
 * [support arm64]
 */
uintptr_t __attribute__((visibility("default"))) tss_get_report_data();

/**
 * @brief Game client calls this interface to release a TssSdkAntiDataInfo struct data which return by tss_get_report_data interface
 */
void __attribute__((visibility("default"))) tss_del_report_data(struct TssSdkAntiDataInfo *data);


void __attribute__((visibility("default"))) tss_enable_get_report_data();


void __attribute__((visibility("default"))) tss_log_str(const char *str);


/**
 * 说明:游戏通过调用tss_sdk_ioctl接口，以类似ioctl函数的使用方式和TssSdk进行交互
 *
 * @request -- 命令字,类型为TssSdkRequestCmdID
 * @param -- 指定命令字对应的输入参数
 * @buf -- 指定命令字对应的用于存放输出结果的buffer
 * @buf_size -- 对应buf的大小
 *
 * 返回值: 根据命令字, 有不同含义
 *
 *
 * TssSdkCmd_GetSdkAntiData:
 *
 *      @brief -- 用于获取TssSdk初始化时产生的数据,一般游戏在用户登录的时候或者对局开始的时候调用该接口获取TssSdk初始化数据，
 *          然后由游戏负责将数据托传给服务端的TssSdk或安全日志。游戏无需关心取到数据的内容，只需完成托传即可。
 *      @要求buf大小 -- 1024
 *      @返回值  -- 0:成功  -1:失败
 *
 * TssSdkCmd_RegistSdkEventListener：
 *
 *      @brief -- 此命令为TssSdk预留命令字,游戏请勿调用
 *      @返回值 -- 0:成功  -1:失败
 *
 * TssSdkCmd_IsAppOnForeground:
 *
 *      @brief -- 用于判断游戏是否在前台
 *      @返回值 -- 1:在前台  0:在后台
 *
  * TssSdkCmd_GetCommLibValueByKey:
 *
 *      @brief -- 获取comm.dat中保存的键值对信息
 *      @返回值 -- 给定key的value
 *
 *  TssSdkCmd_GetShellDyMagicCode:
 *
 *  	@brief -- 获取由加壳系统在加壳的时候给libtersafe.so打入的动态魔码
 *  	@返回值 -- -1:不支持 0或123:未打入魔码 其它(如MMDDHHmm):魔码
 *
 *  TssSdkCmd_AddMTCJTask
 *
 *  	@brief -- 添加云查任务
 *  	@param -- 对应TssOpenMTCJTask结构体的指针
 *  	@返回值 -- 0:成功, 其它失败
 *
 *  	eg.
 *
 *  	TssOpenMTCJTask task = {0};
 *		task.path = "/sdcard/sdk/enable.log";
 *		task.delete_file_after_upload = 0;
 *		task.ignore_flow_limit = 1;
 *
 *		int err = tss_sdk_ioctl(TssSdkCmd_AddMTCJTask, &task, NULL, 0, NULL);
 *		if (err != 0) {
 *			// ..
 *		}
 *
 *	TssSdkCmd_IsEmulator
 *
 *		@brief -- 判断是否为模拟器
 *		@param -- const char *, key:value|key:value ... 形式的字符串
 *		@buf -- 用于存储模拟器名称的buf
 *		@buf_size -- 用于存储模拟器名称的buf的大小
 *		@used_buf_len -- 使用了的buf长度,可为NULL
 *
 *		@返回值 -- 1:为模拟器,模拟器名称放在buf中, 0:不为模拟器, -1:不支持
 *
 *
 *	TssSdkCmd_GameScreenshot2
 *
 *		@brief -- 添加截屏任务
 *		@param -- 对应MrpcsScreenShotTask结构体指针，注意，此结构体中，仅ignore_performance、ignore_flow、extra_param会被使用
 *		@retval -- 返回值:0成功, -1失败
 *
 *		eg
 *
 *		MrpcsScreenShotTask task = {0};
 *		task.ignore_performance = true;	//是否忽略性能
 *		task.ignore_flow = true;		//是否忽略流量限制
 *		task.extra_param = extra_param;
 *
 *		int err = tss_sdk_ioctl(TssSdkCmd_GameScreenshot2, &task, NULL, 0, NULL);
 *		if (err != 0) {
 *			// ..
 *		}
 *
 *	TssSdkCmd_SetToken
 *	    @brief -- 此命令字为单机保护方案功能接口内部保留命令字， 请不要直接使用， 如需使用， 请用tss_sdk_set_token接口
 *	    @返回值 0为成功，-2为失败
 *	
 *	TssSdkCmd_GenSessionData
 *	    @brief -- 此关键字为单机保护方案功能接口关键字， 请不要直接使用， 如需使用， 请用tss_sdk_gen_session_data接口
 *	    @返回值 0为成功, -2为失败
 *
 *	TssSdkCmd_WaitVerify
 *	    @brief -- 此关键字为单机保护方案功能接口关键字， 请不要直接使用， 如需使用， 请用tss_sdk_wait_verify接口
 *	    @返回值 0为成功, -1为失败
 *
 *	TssSDKCmd_CommQuery
 *		@brief -- 通用接口,通过字符串命令,查询返回结果
 *		@返回值 0为成功, -1为失败, 成功时,结果将存于buf参数中
 */
enum TssSdkRequestCmdID
{
    TssSdkCmd_GetSdkAntiData = 1,
    TssSdkCmd_RegistSdkEventListener = 2,
    TssSdkCmd_IsAppOnForeground = 3,
    TssSdkCmd_GameScreenshot= 4,
    TssSdkCmd_InvokeCrashFromShell = 5,
    TssSdkCmd_GetCommLibValueByKey = 6,
    TssSdkCmd_GetShellDyMagicCode = 7,
    TssSdkCmd_AddMTCJTask = 8,
    TssSdkCmd_GameScreenshot2 = 9,
    TssSdkCmd_IsEmulator = 10,
    TssSdkCmd_SetToken = 11,
    TssSdkCmd_GenSessionData = 12,
    TssSdkCmd_WaitVerify = 13,
    TssSdkCmd_QueryTssSdkVer = 14,
    TssSdkCmd_EnableDisableItem = 15,
    TssSdkCmd_QueryOpts = 16,
    TssSdkCmd_ReInitMrpcs = 17,
    TssSDKCmd_CommQuery = 18,
    TssSDKCmd_OpenWBSession = 19,			//参数param为TssWinBrigeTask的指针,结果为WinBridge Session的指针, 将写入param的session成员
    TssSDKCmd_SendWBCmd = 20,				//参数param为TssWinBrigeTask的指针,其中param的session成员为WinBridge Session的指针, cmd为要发送的命令, 调用结果将写入param的response成员
    TssSDKCmd_ReleaseWBStrPool = 21,		//参数param为TssWinBrigeTask的指针,其中param的session成员为WinBridge Session的指针
    TssSDKCmd_CloseWBSession = 22,			//参数param为TssWinBrigeTask的指针,其中param的session成员为WinBridge Session的指针
    TssSDKCmd_GetUserTag = 23,
    TssSDKCmd_QueryTssLibcAddr = 24,
    TssSDKCmd_RegistLibcSendListener = 25,
    TssSDKCmd_RegistLibcRecvListener = 26,
    TssSDKCmd_RegistLibcConnectListener = 27,
    TssSDKCmd_GetMrpcsData2Ptr = 28,
    TssSDKCmd_GetTPChannelVer = 29,
    TssSDKCmd_RecvSecSignature = 34,
};


int __attribute__((visibility("default"))) tss_sdk_ioctl(int request, const void *param, /*[out]*/char *buf, size_t buf_size, /*[out]*/size_t *used_buf_len);

#ifndef TP_MAX_SESSION_DATA_SIZE
#define TP_MAX_SESSION_DATA_SIZE 4096
#endif
/* 说明: 下面为单机保护方案接口
 * @brief
 *      阻塞等待验证. 注意在调用setToken之前不会完成验证。
 *
 * @param timeout 最长等待时间, 单位是ms, 如果设置为0，则无限等待。
 *
 * @return
 *  1  验证不通过
 *  0  验证通过
 *  -1 验证超时
 *  -2 没有收到token
 *  -3 内存不足
 */
int32_t __attribute__((visibility("default"))) tss_sdk_wait_verify(uint32_t timeout);


/*
 * 注意: 这个接口不是线程安全的！
 *
 * @brief
 *  生成sessiondata.
 *
 * @param[out] buff 传入空buff用来接收session_data，请将收到的session_data上传服务端, 用于生成token
 * @param[in] buff_size  传入buff的大小
 *
 * @return
 *  >=0 返回session_data的长度
 *  -1 未登陆
 *  -2 参数错误
 */
int32_t __attribute__((visibility("default"))) tss_sdk_gen_session_data(uint8_t *buff, size_t buff_size);

/*
 * @brief
 *  将服务端返回的token传给验证模块。
 *
 * @param[in] token 服务端返回的token
 * @param[in] n     token长度
 *
 * @return
 * 0 成功
 * -1 没有生成session_data
 * -2 内存不足
 * 
 */
int32_t __attribute__((visibility("default"))) tss_sdk_set_token(const uint8_t *token, size_t n);



/**
 * 获取TssSDK需要通过游戏托传到TssSvrSDK的数据
 *
 * 该接口建议在以下时机点调用，并合并到游戏自身协议的Buffer，然后托传到TssSvrSDK
 *   1.用户登录时
 *   2.游戏开局时
 *   3.游戏结算时
 */
uintptr_t __attribute__((visibility("default"))) tss_get_report_data2();

#ifdef __cplusplus
/**
 * 注册MTP信息订阅者
 *
 * 该接口只能在tss_sdk_init之前调用
 */
void __attribute__((visibility("default"))) tss_sdk_regist_tss_info_receiver(TssInfoReceiver* receiver);
#endif

/*
 * 解密TssInfoReceiver 的onReceive收到的字符串
 */
int  __attribute__((visibility("default"))) tss_sdk_dec_tss_info(const char* src,char* out,size_t len);
    
enum TP2GameStatus
{
    TP2_GAME_STATUS_FRONTEND = 1,  // 前台
    TP2_GAME_STATUS_BACKEND  = 2,  // 后台
};
/**
 * MTP接口兼容,以下接口,只调tp1接口,不作升级接口转交
 */
__attribute__((visibility("default"))) int tp2_sdk_init(int app_id);
__attribute__((visibility("default"))) int tp2_sdk_init_ex(int app_id, const char* app_key);
__attribute__((visibility("default"))) int tp2_setuserinfo(int account_type, int world_id, const char *open_id, const char *role_id);
__attribute__((visibility("default"))) int tp2_getver(char *ver_buf, size_t buf_size);
__attribute__((visibility("default"))) int tp2_setoptions(int options);
__attribute__((visibility("default"))) int tp2_setgamestatus(/*TP2GameStatus*/int status);
__attribute__((visibility("default"))) uintptr_t tp2_sdk_ioctl(int request, const char *param_str);
__attribute__((visibility("default"))) int tp2_free_anti_data(struct TssSdkAntiDataInfo *data);
#ifdef __cplusplus
__attribute__((visibility("default"))) void tp2_regist_tss_info_receiver(TssInfoReceiver* receiver);
#endif
__attribute__((visibility("default"))) int tp2_dec_tss_info(const char* src,char* out,size_t len);


#ifdef __cplusplus
}
#endif


#endif /* TSS_SDK_H */
