#include "tss_data_decrypt2.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "crc32.h"

int tss_data_descrypt2(const char *buf, size_t buf_len, struct TssSdkAntiData2* anti_data)
{
    if (buf == NULL || buf_len == 0 || buf_len > 128 || anti_data == NULL)
    {
        return -1;
    }

	char tmp[132] = {0};
	memcpy(tmp, buf, buf_len);

	sscanf(tmp, "%08X%08X%02X%02X%02X%02X%01X%01X%01X%01X%04X%04X%04X%04X%04X%04X%04X%08X",
			&anti_data->state_word1, &anti_data->state_word2, &anti_data->version, &anti_data->get_cnt,
			&anti_data->scheduel_cnt, &anti_data->scan_cnt, &anti_data->virapp_scan_cnt, &anti_data->ano_scan_cnt,
			&anti_data->lua_scan_cnt, &anti_data->gp5_scan_cnt, &anti_data->remain_cnt,
			&anti_data->pushed_cnt, &anti_data->game_channel_cnt, &anti_data->cs_channel_cnt,
			&anti_data->mrpcs_cnt, &anti_data->crc, &anti_data->pid, &anti_data->slot);


	for (int i = 0; i < 4; i++)
	{
	    tmp[48 + i] = '0';
	}
	unsigned int crc = ssh_crc32((const unsigned char *)tmp, strlen(tmp)) & 0xffff;

	if (crc != anti_data->crc || anti_data->version != 1)
	{
		return -1;
	}

	return 0;
}
