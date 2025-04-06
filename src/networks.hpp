//
// Created by FLN1021 on 2023/9/2.
//

#ifndef PAGER_RECEIVE_NETWORKS_HPP
#define PAGER_RECEIVE_NETWORKS_HPP

#include <WiFi.h>
#include <ctime>
#include <Preferences.h>
#include <PubSubClient.h>
#include "esp_sntp.h"
#include "ESPTelnet.h"
#include <RadioLib.h>
#include "unicon.hpp"
#include "sdlog.hpp"
#include "boards.hpp"
#include "loco.h"
#include "freertos/FreeRTOS.h"

/* ------------------------------------------------ */
#define LBJ_INFO_ADDR 1234000
#define LBJ_INFO2_ADDR 1234002
#define LBJ_SYNC_ADDR 1234008

#define TARGET_FREQ 821.2375 // MHz

#define FUNCTION_DOWN 1
#define FUNCTION_UP 3

#define POCDAT_SIZE 16 // defines number of the pocsag data structures.

struct lbj_data
{
    int8_t type = -1;
    char train[6] = "<NUL>";
    int8_t direction = -1;
    char speed[6] = "NUL";
    char position[7] = " <NUL>";
    char time[6] = "<NUL>";
    String info2_hex;
    String loco_type;
    //    char info2_hex[51] = "<NUL>";
    char lbj_class[3] = "NA";    // '0X' or ' X'
    char loco[9] = "<NUL>";      // such as 23500331
    char route[17] = "********"; // 16 bytes GBK data.
    char route_utf8[17 * 2] = "********";
    char pos_lon_deg[4] = ""; // ---
    char pos_lon_min[8] = ""; // --.----
    char pos_lat_deg[3] = ""; // --
    char pos_lat_min[8] = "";
    char pos_lon[10] = "<NUL>"; // ---°--.----'
    char pos_lat[9] = "<NUL>";  // --°--.----'
};

struct rx_info
{
    float rssi = 0;
    float fer = 0;
    float ppm = 0;
    uint16_t cnt = 0;
    uint64_t timer = 0;
};

struct data_bond
{
    PagerClient::pocsag_data pocsagData[POCDAT_SIZE];
    lbj_data lbjData;
    String str;
};
/* ------------------------------------------------ */

extern const char *time_zone;
extern const char *ntpServer1;
extern const char *ntpServer2;

extern struct tm time_info;

#define NETWORK_TIMEOUT 1800000 // 30 minutes

extern ESPTelnet telnet;
extern IPAddress ip;
extern uint16_t port;
extern bool is_startline;
extern SD_LOG sd1;
extern bool give_tel_rssi;
extern bool give_tel_gain;
extern bool tel_set_ppm;
extern bool no_wifi;
extern float actual_frequency;
extern uint64_t prb_timer;
extern uint32_t prb_count;
extern float ppm;
extern bool freq_correction;
extern bool telnet_online;

bool isConnected();

bool connectWiFi();

void performSmartConfig();

bool connectToWiFi(const String &ssid, const String &password);

bool connectMQTT();

void silentConnect(const char *ssid, const char *password);

void changeCpuFreq(uint32_t freq_mhz);

void timeAvailable(struct timeval *t);

void timeSync(struct tm &time);

char *fmtime(const struct tm &time);

char *fmtms(uint64_t ms);

void onTelnetConnect(String ip);

void onTelnetDisconnect(String ip);

void onTelnetReconnect(String ip);

void onTelnetConnectionAttempt(String ip);

void onTelnetInput(String str);

void setupTelnet();

void timeTask(void *pVoid);

// extern bool ipChanged(uint16_t interval);

int16_t readDataLBJ(struct PagerClient::pocsag_data *p, struct lbj_data *l);

int8_t hexToChar(int8_t hex1, int8_t hex2);

void recodeBCD(const char *c, String *v);

int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput);

void gbk2utf8(const uint8_t *gbk, uint8_t *utf8, size_t gbk_len);

void gbk2utf8(const char *gbk1, char *utf8s, size_t gbk_len);

void telPrintf(bool time_stamp, const char *format, ...);

void telPrintLog(int chars);

void printDataSerial(PagerClient::pocsag_data *p, const struct lbj_data &l, const struct rx_info &r);

void appendDataLog(PagerClient::pocsag_data *p, const struct lbj_data &l, const struct rx_info &r);

void printDataTelnet(PagerClient::pocsag_data *p, const struct lbj_data &l, const struct rx_info &r);

void appendDataCSV(PagerClient::pocsag_data *p, const struct lbj_data &l, const struct rx_info &r);

float getBias(float freq);

#ifdef HAS_RTC
tm rtcLibtoC(const DateTime &datetime);

DateTime rtcLibtoC(const tm &ctime);
#endif

#endif // PAGER_RECEIVE_NETWORKS_HPP
