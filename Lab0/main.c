/**
 * main.c
 *
 * ECE 3849 Lab 0 Starter Project
 * Gene Bogdanov    10/18/2017
 *
 * This version is using the new hardware for B2017: the EK-TM4C1294XL LaunchPad with BOOSTXL-EDUMKII BoosterPack.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include "buttons.h"

uint32_t gSystemClock; // [Hz] system clock frequency
volatile uint32_t gTime = 8345; // time in hundredths of a second

int main(void)
{
    IntMasterDisable();

    // Enable the Floating Point Unit, and permit ISRs to use it
    FPUEnable();
    FPULazyStackingEnable();

    // Initialize the system clock to 120 MHz
    gSystemClock = SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, 120000000);

    Crystalfontz128x128_Init(); // Initialize the LCD display driver
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP); // set screen orientation

    tContext sContext;
    GrContextInit(&sContext, &g_sCrystalfontz128x128); // Initialize the grlib graphics context
    GrContextFontSet(&sContext, &g_sFontFixed6x8); // select font

    ButtonInit(); //Initialize buttons and joystick

    IntMasterEnable();//Re-enable interupts for running

    uint32_t time;  // local copy of gTime
    uint32_t huns;
    uint32_t secs;
    uint32_t mins;
    uint32_t buttons_read;
    int i;
    char str[50];   // string buffer
    char str_debug[50];
    int buttons[9]; //array to hold the bitmap
    // full-screen rectangle
    tRectangle rectFullScreen = {0, 0, GrContextDpyWidthGet(&sContext)-1, GrContextDpyHeightGet(&sContext)-1};

    while (true) {
        GrContextForegroundSet(&sContext, ClrBlack);
        GrRectFill(&sContext, &rectFullScreen); // fill screen with black
        time = gTime; // read shared global only once
        buttons_read = gButtons;
        huns = time%100;
        secs = (time/100)%60;
        mins = (time/6000);
        for (i = 0; i < 9 ; i++){
            buttons[i] = (buttons_read & (1 << i)) >> i;
        }
        snprintf(str, sizeof(str), "Time = %02u:%02u:%02u",mins,secs,huns); // convert time to string
        snprintf(str_debug, sizeof(str), "Buttons = %d%d%d%d%d%d%d%d%d", buttons[8], buttons[7], buttons[6], buttons[5], buttons[4], buttons[3], buttons[2], buttons[1], buttons[0]);
        GrContextForegroundSet(&sContext, ClrYellow); // yellow text
        GrStringDraw(&sContext, str, /*length*/ -1, /*x*/ 0, /*y*/ 0, /*opaque*/ false);
        GrStringDraw(&sContext, str_debug, /*length*/ -1, /*x*/ 0, /*y*/ 12, /*opaque*/ false);
        GrFlush(&sContext); // flush the frame buffer to the LCD
    }
}
