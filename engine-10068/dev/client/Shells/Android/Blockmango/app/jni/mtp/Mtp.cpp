//
// Created by luo on 2020/8/3.
//

#include "Mtp.h"
#include "tp2_sdk.h"
#include <android/log.h>
#include "unistd.h"
#include <pthread.h>
#include <errno.h>
#include "tss_data_decrypt2.h"


MtpTssInfoReceiver::MtpTssInfoReceiver()
{
    m_blackList.push_back("com.claroecuador.miclaro");
    m_blackList.push_back("br.com.nextel.ecommerce");
    m_blackList.push_back("catch_.me_.if_.you_.can_");
    m_blackList.push_back("com.fjhybbjfjjuryhuuik.p");
    m_blackList.push_back("com.d.esqvxo.faazilbi");
    m_blackList.push_back("com.landlordgame.tycoon");
    m_blackList.push_back("com.hackgaming");
    m_blackList.push_back("com.vnpqk");
    m_blackList.push_back("com.cri.smartad");
    m_blackList.push_back("com.otecel.B2B.MiMovistar");
    m_blackList.push_back("com.otecel.mimovistar");
    m_blackList.push_back("com.gmg.id");
    m_blackList.push_back("com.bdlr.sq.cfhgqwwxmska");
    m_blackList.push_back("com.surcumference.xsposed");
    m_blackList.push_back("com.skzfueimaivbtp");
    m_blackList.push_back("com.jol");
    m_blackList.push_back("com.ucwfghikf");
    m_blackList.push_back("com.wokvmxsefm");
    m_blackList.push_back("com.gfbhfkfwvbht");
    m_blackList.push_back("com.bmwdwgocfadecymdkglu");
    m_blackList.push_back("com.ymp");
    m_blackList.push_back("cf.androefi.xenone");

}

void MtpTssInfoReceiver::onReceive(int tssInfoType, const char* info)
{
    char plain[256];
    memset(plain, 0x00, sizeof(plain));
    if (tssInfoType == TSS_INFO_TYPE_DETECT_RESULT)
    {
        // 只关注TSS_INFO_TYPE_DETECT_RESULT
        int ret =tp2_dec_tss_info(info,plain,sizeof(plain));
        if (ret == -1) return;

        __android_log_print(ANDROID_LOG_DEBUG, "Main", "[C++  Cheat Info 0] : %d | %s", tssInfoType, plain);

        std::string str(info);
        if (!findStr(str, "id=9") && !findStr(str, "id=10") && !findStr(str, "id=13")) {
            if(isInBlackList(str)) {
                __android_log_print(ANDROID_LOG_DEBUG, "Main" , "1 kill game on black list.");
                unsigned int pid = getpid();
                pthread_kill(pid, SIGQUIT);
            }
            return;
        }

    }
    else if (tssInfoType == TSS_INFO_TYPE_HEARTBEAT)
    {
        // // 处理心跳，如果不关心，可以忽略
        int ret = tp2_dec_tss_info(info, plain, sizeof(plain));
        if (ret == -1) return;
        __android_log_print(ANDROID_LOG_DEBUG, "Main", "[C++  Cheat Info 1 ] : %d | %s", tssInfoType,plain);


        TssSdkAntiDataInfo *anti_data =  (TssSdkAntiDataInfo*)tss_get_report_data2();
        if(anti_data != NULL && anti_data->anti_data_len_<= 128)
        {
            TssSdkAntiData2 antiData;
            char data[anti_data->anti_data_len_];
            for(int i = 0; i < anti_data->anti_data_len_; i++) {
                data[i] = anti_data->anti_data_[i];
            }
            int code = tss_data_descrypt2(data, anti_data->anti_data_len_, &antiData);
            if(code == 0) {
                if(((antiData.state_word1 & AntiDataMagicFlag2_UpdateCDNRunned == 0) &&
                    (antiData.state_word1 & AntiDataMagicFlag2_ScheduleEngIsRunnig == 0) &&
                    (antiData.state_word1 & AntiDataMagicFlag2_CommDatDownloaded == 0) && antiData.get_cnt >= 10)){
                    __android_log_print(ANDROID_LOG_DEBUG, "Main" , "2 kill game on data check.");
                    unsigned int pid = getpid();
                    //pthread_kill(pid, SIGQUIT);
                }
            }
        }
    }
}

bool MtpTssInfoReceiver::isInBlackList(std::string info)
{
    for (const char* packName : m_blackList)
    {
        if (info.find(packName)!= info.npos)
        {
            return true;
        }
    }

    if(findStr(info, "id=7") && findStr(info, "reason=none"))
    {
        return true;
    }
	
	if(findStr(info, "id=1") && (findStr(info, "reason=com.xy") || findStr(info, "reason=com.cdt.jl")))
    {
        return true;
    }

    if(findStr(info, "id=3") && (findStr(info, "gp4_ignore_sig") || findStr(info, "reason=604")))
    {
       return true;
    }

    return false;
}

bool MtpTssInfoReceiver::findStr(std::string main, std::string child)
{
    return main.find(child.c_str()) != main.npos;
}

Mtp::Mtp()
{
}

void Mtp::init(const char* userId)
{
    if (!isInit)
    {
        isInit = true;
        tp2_regist_tss_info_receiver(&m_tssInfoReceiver);
        tp2_sdk_ioctl(TssSDKCmd_CommQuery, "MTPDefenceLevel:M");
        int code1 = tp2_sdk_init_ex (19778, "f3d78e41be8194398218601ec9c19655");
        int code2 = tp2_setuserinfo(99, 0, userId, "");
        __android_log_print(ANDROID_LOG_DEBUG, "Main", "[C++ init]");
    }
}

void Mtp::onResume()
{
    int ret = tp2_setgamestatus(TP2_GAME_STATUS_FRONTEND);

    // 以下代码仅用于验证MTP接口调用是否正确，当info->anti_data_len_包含字符串"tp2 interface success"时，表明接口调用正确
    TssSdkAntiDataInfo *info = (TssSdkAntiDataInfo *)tp2_sdk_ioctl(TssSDKCmd_CommQuery,"GetSDKInterfaceChkState");

    if (info != NULL && info->anti_data_len_ > 0)
    {
        if (strcmp((const char*)info->anti_data_, "tp2 interface success") == 0)
        {
            // 接口接入成功
            __android_log_print(ANDROID_LOG_ERROR,"Main","MTP interface success");
        }
        else
        {
            __android_log_print(ANDROID_LOG_ERROR,"Main","MTP %s", info->anti_data_);
        }
        tp2_free_anti_data(info);
    }
    else
    {
        __android_log_print(ANDROID_LOG_ERROR,"Main","MTP tp2_sdk_ioctl not implement");
    }
}

void Mtp::onPause()
{
    tp2_setgamestatus(TP2_GAME_STATUS_BACKEND);
}
