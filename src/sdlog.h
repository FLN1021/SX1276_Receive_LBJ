//
// Created by FLN1021 on 2023/9/5.
//

#ifndef PAGER_RECEIVE_SDLOG_H
#define PAGER_RECEIVE_SDLOG_H

#include <SD.h>
#include <FS.h>
#include <Arduino.h>
#include <ctime>
#include "utilities.h"
#include "ESPTelnet.h"
#include "boards.h"

#define MAX_LOG_SIZE 500000 // 500000 default
#define MAX_CSV_SIZE 500000

class SD_LOG {
public:
    // SD_LOG() = default;
    SD_LOG();

    // explicit SD_LOG(fs::FS &fs);
    void setFS(fs::FS &fs);

    int begin(const char *path);

    int beginCSV(const char *path);

    void getFilenameCSV(const char *path);

    void append(const char *format, ...);

    void appendCSV(const char *format, ...);

    void appendBuffer(const char *format, ...);

    void sendBufferLOG();

    void appendBufferCSV(const char *format, ...);

    void sendBufferCSV();

    void printTel(int chars, ESPTelnet &tel);

    File logFile(char op);

    void reopen();

    void disableSizeCheck();

    void enableSizeCheck();

    bool status() const;

private:
    void getFilename(const char *path);

    void writeHeader();

    void writeHeaderCSV();

    String log_path;
    String csv_path;
    fs::FS *filesys;
    String large_buffer;
    String large_buffer_csv;
    File log;
    File csv;
    int log_count; // Actual file count - 1. =0 default
    char filename[32]; // =""
    char filename_csv[32];
    bool sd_log; // = false
    bool sd_csv;
//    bool have_sd = false;
//    bool haveNTP = false;
    bool is_newfile; // = false
    bool is_startline; // = true
    bool size_checked;
    bool is_newfile_csv;
    bool is_startline_csv;
    const char *log_directory;
    const char *csv_directory;

    struct tm timein{};
};


#endif //PAGER_RECEIVE_SDLOG_H
