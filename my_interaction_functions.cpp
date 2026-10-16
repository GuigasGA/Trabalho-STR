#include "my_interaction_functions.h"

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
#include <interface.h>
}

// ============================================================
// Bit manipulation helpers
// ============================================================

int getBitValue(uInt8 value, uInt8 bit_n)
{
    return(value & (1 << bit_n));
}

void setBitValue(uInt8* variable, int n_bit, int new_value_bit)
{
    uInt8 mask_on = (uInt8)(1 << n_bit);
    uInt8 mask_off = ~mask_on;
    if (new_value_bit) *variable |= mask_on;
    else *variable &= mask_off;
}

// ============================================================
// X axis
// ============================================================

void moveXLeft()
{
    uInt8 p = readDigitalU8(2);
    setBitValue(&p, 6, 1);
    setBitValue(&p, 7, 0);
    writeDigitalU8(2, p);
}

void moveXRight()
{
    uInt8 p = readDigitalU8(2);
    setBitValue(&p, 6, 0);
    setBitValue(&p, 7, 1);
    writeDigitalU8(2, p);
}

void stopX()
{
    uInt8 p = readDigitalU8(2);
    setBitValue(&p, 6, 0);
    setBitValue(&p, 7, 0);
    writeDigitalU8(2, p);
}

int getXPos()
{
    uInt8 p = readDigitalU8(0);
    if (!getBitValue(p, 2)) return 1;
    if (!getBitValue(p, 1)) return 2;
    if (!getBitValue(p, 0)) return 3;
    return(-1);
}

void gotoX(int x)
{
    int currentPos = getXPos();

    if (x == 1)
    {
        while (getXPos() != 1) { moveXLeft(); vTaskDelay(1); }
        stopX();
    }
    else if (x == 2)
    {
        if (currentPos < 2) {
            while (getXPos() != 2) { moveXRight(); vTaskDelay(1); }
        }
        else {
            while (getXPos() != 2) { moveXLeft(); vTaskDelay(1); }
        }
        stopX();
    }
    else if (x == 3)
    {
        while (getXPos() != 3) { moveXRight(); vTaskDelay(1); }
        stopX();
    }
}

// ============================================================
// Y axis
// ============================================================

void moveYInside()
{
    uInt8 p = readDigitalU8(2);
    setBitValue(&p, 5, 1);
    setBitValue(&p, 4, 0);
    writeDigitalU8(2, p);
}

void moveYOutside()
{
    uInt8 p = readDigitalU8(2);
    setBitValue(&p, 5, 0);
    setBitValue(&p, 4, 1);
    writeDigitalU8(2, p);
}

void stopY()
{
    uInt8 p = readDigitalU8(2);
    setBitValue(&p, 5, 0);
    setBitValue(&p, 4, 0);
    writeDigitalU8(2, p);
}

int getYPos()
{
    uInt8 p = readDigitalU8(0);
    if (!getBitValue(p, 5)) return 1;
    if (!getBitValue(p, 4)) return 2;
    if (!getBitValue(p, 3)) return 3;
    return(-1);
}

void gotoY(int y)
{
    int currentPos = getYPos();

    if (y == 1)
    {
        while (getYPos() != 1) { moveYOutside(); vTaskDelay(1); }
        stopY();
    }
    else if (y == 2)
    {
        if (currentPos < 2) {
            while (getYPos() != 2) { moveYInside(); vTaskDelay(1); }
        }
        else {
            while (getYPos() != 2) { moveYOutside(); vTaskDelay(1); }
        }
        stopY();
    }
    else if (y == 3)
    {
        while (getYPos() != 3) { moveYInside(); vTaskDelay(1); }
        stopY();
    }
}

// ============================================================
// Z axis
// ============================================================

void moveZUp()
{
    uInt8 p = readDigitalU8(2);
    setBitValue(&p, 3, 1);
    setBitValue(&p, 2, 0);
    writeDigitalU8(2, p);
}

void moveZDown()
{
    uInt8 p = readDigitalU8(2);
    setBitValue(&p, 3, 0);
    setBitValue(&p, 2, 1);
    writeDigitalU8(2, p);
}

void stopZ()
{
    uInt8 p = readDigitalU8(2);
    setBitValue(&p, 3, 0);
    setBitValue(&p, 2, 0);
    writeDigitalU8(2, p);
}

int getZPos()
{
    uInt8 p0 = readDigitalU8(0);
    uInt8 p1 = readDigitalU8(1);
    if (!getBitValue(p1, 3)) return 1;
    if (!getBitValue(p1, 1)) return 2;
    if (!getBitValue(p0, 7)) return 3;

    if (!getBitValue(p1, 2)) return 4;
    if (!getBitValue(p1, 0)) return 5;
    if (!getBitValue(p0, 6)) return 6;

    return(-1);
}

void gotoZ(int z)
{
    int currentPos = getZPos();

    if (z == 1)
    {
        while (getZPos() != 1) { moveZDown(); vTaskDelay(1); }
        stopZ();
    }
    else if (z == 2)
    {
        if (currentPos < 2) {
            while (getZPos() != 2) { moveZUp(); vTaskDelay(1); }
        }
        else {
            while (getZPos() != 2) { moveZDown(); vTaskDelay(1); }
        }
        stopZ();
    }
    else if (z == 3)
    {
        while (getZPos() != 3) { moveZUp(); vTaskDelay(1); }
        stopZ();
    }
}

// ============================================================
// Z half-step helpers
// ============================================================

void gotoZUp()
{
    int currentPos = getZPos();
    if (currentPos == 1) {
        while (getZPos() != 4) { moveZUp(); vTaskDelay(1); }
        stopZ();
    }
    else if (currentPos == 2) {
        while (getZPos() != 5) { moveZUp(); vTaskDelay(1); }
        stopZ();
    }
    else if (currentPos == 3) {
        while (getZPos() != 6) { moveZUp(); vTaskDelay(1); }
        stopZ();
    }
}

void gotoZDown()
{
    int currentPos = getZPos();
    if (currentPos == 4) {
        while (getZPos() != 1) { moveZDown(); vTaskDelay(1); }
        stopZ();
    }
    else if (currentPos == 5) {
        while (getZPos() != 2) { moveZDown(); vTaskDelay(1); }
        stopZ();
    }
    else if (currentPos == 6) {
        while (getZPos() != 3) { moveZDown(); vTaskDelay(1); }
        stopZ();
    }
}

// ============================================================
// Composite operations
// ============================================================

void putPartInCell()
{
    gotoZUp();
    gotoY(3);
    gotoZDown();
    gotoY(2);
}

void takePartFromCell()
{
    gotoY(3);
    gotoZUp();
    gotoY(2);
    gotoZDown();
}

// ============================================================
// ISRs dos botões
// ============================================================

void switch1_rising_isr(ULONGLONG lastTime) {
    // GetTickCount64() current time in miliseconds
    // since the system has started...
    ULONGLONG time = GetTickCount64();
    printf("\nSwitch one RISING detected at time = %llu...\n", time);

    // Contagem de pulsos
    g_pulseUpper++;
    g_lastUpperTime = time;

    // TODO (Fase 4.6 — STOP/RESUME):
    // BaseType_t xYieldRequired;
    // xYieldRequired = xTaskResumeFromISR(emergencyTask);
}



void switch1_falling_isr(ULONGLONG lastTime) {
    ULONGLONG time = GetTickCount64();
    printf("\nSwitch one FALLING detected at time = %llu...\n", time);
    // Nada a fazer — serve para debug e para o futuro RESET/STOP
}



void switch2_rising_isr(ULONGLONG lastTime) {
    ULONGLONG time = GetTickCount64();
    printf("\nSwitch two RISING detected at time = %llu...\n", time);

    // Contagem de pulsos (necessário para FR4)
    g_pulseLower++;
    g_lastLowerTime = time;

    // TODO (Fase 4.6 — RESUME/RESET):
    // BaseType_t xYieldRequired;
    // xYieldRequired = xTaskResumeFromISR(resumeTask);
}
