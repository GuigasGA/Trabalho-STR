#include "my_interaction_functions.h"

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
#include <interface.h>
#include <semphr.h>
}

extern volatile bool resetInProgress;

// ============================================================
// Bit manipulation
// ============================================================
int getBitValue(uInt8 value, uInt8 bit_n) {
    return(value & (1 << bit_n));
}

void setBitValue(uInt8* variable, int n_bit, int new_value_bit) {
    uInt8 mask_on = (uInt8)(1 << n_bit);
    uInt8 mask_off = ~mask_on;
    if (new_value_bit) *variable |= mask_on;
    else *variable &= mask_off;
}

static bool moveTimedOut(ULONGLONG start, ULONGLONG maxMs, const char* fn) {
    if (GetTickCount64() - start > maxMs) {
        printf("[%s] TIMEOUT\n", fn);
        return true;
    }
    return false;
}

// ============================================================
// X axis
// ============================================================
void moveXLeft()
{
    taskENTER_CRITICAL();
    uInt8 p = readDigitalU8(2); 
    setBitValue(&p, 6, 1);     
    setBitValue(&p, 7, 0);     
    writeDigitalU8(2, p);       
    taskEXIT_CRITICAL();
}

void moveXRight()
{
    taskENTER_CRITICAL();
    uInt8 p = readDigitalU8(2); 
    setBitValue(&p, 6, 0);     
    setBitValue(&p, 7, 1);    
    writeDigitalU8(2, p);     
    taskEXIT_CRITICAL();
}

void stopX()
{
    taskENTER_CRITICAL();
    uInt8 p = readDigitalU8(2); 
    setBitValue(&p, 6, 0);     
    setBitValue(&p, 7, 0);      
    writeDigitalU8(2, p);       
    taskEXIT_CRITICAL();
}

int getXPos()
{
    uInt8 p = readDigitalU8(0);
    if (!getBitValue(p, 2))
        return 1;
    if (!getBitValue(p, 1))
        return 2;
    if (!getBitValue(p, 0))
        return 3;
    return(-1);
}

void gotoX(int x)
{
    int pos = getXPos();
    if (pos == -1) {
        while (getXPos() == -1) {
            moveXLeft();
            taskYIELD();
        }
        stopX();
        pos = getXPos(); 
    }

    if (pos == x) { 
        printf("\nAlready at X position %d", x);
    }
    else if (pos < x) {
        while (getXPos() != x) {
            moveXRight();
            taskYIELD();
        }
        stopX();
    }
    else {
        while (getXPos() != x) {
            moveXLeft();
            taskYIELD();
        }
        stopX();
    }
}
// ============================================================
// Y axis
// ============================================================
void moveYOutside()
{
    taskENTER_CRITICAL();
    uInt8 p = readDigitalU8(2);
    setBitValue(&p, 4, 1);     
    setBitValue(&p, 5, 0);     
    writeDigitalU8(2, p);       
    taskEXIT_CRITICAL();
}

void moveYInside()
{
    taskENTER_CRITICAL();
    uInt8 p = readDigitalU8(2); 
    setBitValue(&p, 4, 0);     
    setBitValue(&p, 5, 1);      
    writeDigitalU8(2, p);     
    taskEXIT_CRITICAL();
}

void stopY()
{
    taskENTER_CRITICAL();
    uInt8 p = readDigitalU8(2); 
    setBitValue(&p, 4, 0);    
    setBitValue(&p, 5, 0);      
    writeDigitalU8(2, p);       
    taskEXIT_CRITICAL();
}

int getYPos()
{
    uInt8 p = readDigitalU8(0);
    if (!getBitValue(p, 5))
        return 1;
    if (!getBitValue(p, 4))
        return 2;
    if (!getBitValue(p, 3))
        return 3;
    return(-1);
}

void gotoY(int y)
{
    int pos = getYPos();
    if (pos == -1) {
        while (getYPos() == -1) {
            moveYOutside();
            taskYIELD();
        }
        stopY();
        pos = getYPos();
    }
    if (pos == y) {
        printf("\nAlready at Y position %d", y);
    }
    else if (pos < y) {
        while (getYPos() != y) {
            moveYInside();
            taskYIELD();
        }

        stopY();
    }
    else {
        while (getYPos() != y) {
            moveYOutside();
            taskYIELD();
        }
        stopY();
    }
}
// ============================================================
// Z axis
// ============================================================
void moveZUp()
{
    taskENTER_CRITICAL();
    uInt8 p = readDigitalU8(2); 
    setBitValue(&p, 3, 1);      
    setBitValue(&p, 2, 0);      
    writeDigitalU8(2, p);       
    taskEXIT_CRITICAL();
}

void moveZDown()
{
    taskENTER_CRITICAL();
    uInt8 p = readDigitalU8(2); 
    setBitValue(&p, 3, 0);     
    setBitValue(&p, 2, 1);      
    writeDigitalU8(2, p);      
    taskEXIT_CRITICAL();
}

void stopZ()
{
    taskENTER_CRITICAL();
    uInt8 p = readDigitalU8(2); 
    setBitValue(&p, 3, 0);      
    setBitValue(&p, 2, 0);     
    writeDigitalU8(2, p);       
    taskEXIT_CRITICAL();
}

int getZPos()
{
    uInt8 p0 = readDigitalU8(0);
    uInt8 p1 = readDigitalU8(1);

    if (!getBitValue(p1, 3))
        return 1;
    if (!getBitValue(p1, 1))
        return 2;
    if (!getBitValue(p0, 7))
        return 3;
    return(-1);
}

void gotoZ(int z)
{
    if (getZPos() == -1)
    {
        while (getZPos() == -1)
        {
            moveZDown();
            taskYIELD();
        }
        stopZ();
    }

    int pos = getZPos();

    if (pos == z)
    {
        printf("\nAlready at Z position %d", z);
    }
    else if (pos < z)
    {
        while (getZPos() != z)
        {
            moveZUp();
            taskYIELD();
        }
        stopZ();
    }
    else
    {
        while (getZPos() != z) 
        {
            moveZDown();
            taskYIELD();
        }
        stopZ();
    }
}

void gotoZUp()
{
    moveZUp();
    while (true)
    {
        uInt8 p0 = readDigitalU8(0);
        uInt8 p1 = readDigitalU8(1);

        if (!getBitValue(p1, 2) || !getBitValue(p1, 0) || !getBitValue(p0, 6)) {
            break;
        }
        taskYIELD();
    }
    stopZ();
}

void gotoZDown()
{
    moveZDown();
    while (true)
    {
        uInt8 p0 = readDigitalU8(0);
        uInt8 p1 = readDigitalU8(1);

        if (!getBitValue(p1, 3) || !getBitValue(p1, 1) || !getBitValue(p0, 7)) {
            break;
        }
        taskYIELD();
    }
    stopZ();
}

void putPartInCell() {
    gotoZUp();
    gotoY(3);
    gotoZDown();
    gotoY(2);
}

void takePartFromCell() {
    gotoY(3);
    gotoZUp();
    gotoY(2);
    gotoZDown();
}
