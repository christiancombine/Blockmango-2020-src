#ifndef __TSS_DATA_DECRYPT2_H__
#define __TSS_DATA_DECRYPT2_H__

#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)

enum AntiDataMagicFlag2
{
    AntiDataMagicFlag2_InitCalled = 0x00000001,                  //是否调用了初始化接口，可以作为鸡腿等外挂通过不让游戏调用初始化接口导致全超时的依据
    AntiDataMagicFlag2_SetUserInfoCalled = 0x00000002,           //是否调用了用户登录接口，也可以作为鸡腿等外挂通过不让游戏调用初始化接口导致全超时的依据
    AntiDataMagicFlag2_ScanEngineRunned = 0x00000004,            //SDK的关键扫描引擎是否运行，如果没有运行，SDK的关键扫描都不会执行，大概率是被外挂攻击了
    AntiDataMagicFlag2_UpdateCdnOk = 0x00000008,                 //特征下载是否成功，只要有一个特征下载不成功，就认为失败
    AntiDataMagicFlag2_CsChannelOk = 0x00000010,                 //CS通道是否连通，如果有包通过CS通道发送成功了，就算成功
    AntiDataMagicFlag2_Gp3Runned = 0x00000040,                   //GP3模块是否启动
    AntiDataMagicFlag2_Gp4Runned = 0x00000080,                   //GP4模块是否启动
    AntiDataMagicFlag2_IsEmulator = 0x00001000,                  //是否为模拟器，同9091上报
    AntiDataMagicFlag2_IsCacheWritable = 0x00002000,             //files目录是否可写，外网以前会通过让files不可写来导致GP3启动失败，从而使用GG修改器
    AntiDataMagicFlag2_IsX86 =  0x00004000,                      //是否为X86设备，包括X86真机
    AntiDataMagicFlag2_ScheduleEngIsRunnig = 0x00008000,         //调度引擎是否正在运行，如果没有运行，则SDK的主要模块都会失效，大概率是被外挂攻击了
    AntiDataMagicFlag2_CsChannelConnectStateIsOk = 0x00010000,   //CS通道线程当前是否运行中，如果没有运行，说明CS通道连接不成功，无法通过CS通道发包
    AntiDataMagicFlag2_OnForeGround = 0x00020000,                //游戏是否在前台运行
    AntiDataMagicFlag2_Gp4IsRunning = 0x00040000,                //GP4模块是否正在运行中
    AntiDataMagicFlag2_CommDatDownloaded = 0x00080000,           //SDK通用特征comm.dat是否下载成功
    AntiDataMagicFlag2_LuaRunned = 0x00100000,                   //lua扫描是否执行完毕
    AntiDataMagicFlag2_VapRunned = 0x00200000,                   //虚拟机扫描是否执行完毕
    AntiDataMagicFlag2_EmuRunned = 0x00400000,                   //模拟器扫描是否执行完毕
    AntiDataMagicFlag2_UpdateCDNRunned = 0x00800000,             //特征更新是否执行过
    AntiDataMagicFlag2_IsRoot = 0x01000000,                      //设备是否root或越狱
    AntiDataMagicFlag2_VirApp = 0x02000000,                      //是否运行在虚拟机里面
    AntiDataMagicFlag2_SensorUnchanged = 0x10000000,             //传感器是否未变化
};

struct TssSdkAntiData2
{
	uint32_t state_word1;       //SDK各模块状态信息，具体状态见AntiDataMagicFlag2
	uint32_t state_word2;
	uint8_t version;            //版本号
	uint8_t get_cnt;
	uint8_t scheduel_cnt;
	uint8_t scan_cnt;
	uint8_t virapp_scan_cnt;
	uint8_t ano_scan_cnt;
	uint8_t lua_scan_cnt;
	uint8_t gp5_scan_cnt;
	uint16_t remain_cnt;
	uint16_t pushed_cnt;
	uint16_t game_channel_cnt;
	uint16_t cs_channel_cnt;
	uint16_t mrpcs_cnt;
	uint16_t crc;               //本数据CRC
	uint16_t pid;
	uint32_t slot;
};

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

int tss_data_descrypt2(const char *buf, size_t buf_len, struct TssSdkAntiData2* anti_data);

#ifdef __cplusplus
}
#endif

#endif
