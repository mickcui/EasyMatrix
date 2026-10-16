#ifndef __LIGHT_H
#define __LIGHT_H

#include "common.h"
#include <Adafruit_NeoMatrix.h>

extern int clockColor[3];
extern int brightness;
extern uint16_t mainColor;
extern uint16_t weekColor;
extern Adafruit_NeoMatrix matrix;
extern int timePage;
extern int rhythmPage;
extern int animPage;
extern int rhythmBandsModel;
extern int timeModel;
extern int brightModel;
extern int brightSamplingTime;
extern int brightSamplingValue;
extern int matrixArray[13][32];
extern byte peak[NUM_BANDS];
extern int oldBarHeights[NUM_BANDS];
extern int lightedCount;
extern bool drawTimeFirstTime;
extern int timeIndex;
void initMatrix();
void drawWifiText();
void drawText(int x, int y, String text);
void drawCheckTimeText();
void drawFailed(int textX, int failedX, String text);
void clearMatrix();
void drawTime();
void showIp();
void drawBright();
void drawAnim();
void drawRHYTHM();
void calculateBrightnessValue();
void clearBrightSampling();
extern bool isShowingMarquee;
void startMarquee(String text, int repeats = 1);
void drawMarqueeFrame();

#endif

