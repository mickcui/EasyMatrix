#ifndef __TASK_H
#define __TASK_H

#include <Arduino.h>
#include "common.h"
#include "Ticker.h"

extern enum CurrentPage currentPage;
extern TaskHandle_t showTextTask;
extern TaskHandle_t showIpTask;
void safeDeleteTask(TaskHandle_t &handle);
void watchBtn();
void createShowTextTask(char *text);
void createShowIpTask();
void btnInit();
extern Ticker tickerCheckTime;
extern bool isCheckingTime;
void startTickerCheckTime();

#endif

