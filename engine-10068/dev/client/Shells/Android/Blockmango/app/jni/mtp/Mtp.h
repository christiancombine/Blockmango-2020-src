//
// Created by luo on 2020/8/3.
//

#ifndef BLOCKMANGO_MTP_H
#define BLOCKMANGO_MTP_H
#include <string>
#include <stdio.h>
#include <vector>
#include "tp2_sdk.h"


class MtpTssInfoReceiver : public TssInfoReceiver
{

public:
    MtpTssInfoReceiver();
    virtual void onReceive(int tssInfoType, const char* info);

private:
    std::vector<char*> m_blackList;
    bool isInBlackList(std::string info);
    bool findStr(std::string main,  std::string child);
};

class Mtp
{
public:
	Mtp();
	~Mtp() {};
	void init(const char* userId);
	void onResume();
	void onPause();

private:
    bool isInit = false;
    MtpTssInfoReceiver m_tssInfoReceiver;
};


#endif //BLOCKMANGO_MTP_H
