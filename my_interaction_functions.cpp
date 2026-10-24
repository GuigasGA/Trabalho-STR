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
void moveXLeft() { uInt8 p = readDigitalU8(2); setBitValue(&p, 6, 1); setBitValue(&p, 7, 0); writeDigitalU8(2, p); }
void moveXRight() { uInt8 p = readDigitalU8(2); setBitValue(&p, 6, 0); setBitValue(&p, 7, 1); writeDigitalU8(2, p); }
void stopX() { uInt8 p = readDigitalU8(2); setBitValue(&p, 6, 0); setBitValue(&p, 7, 0); writeDigitalU8(2, p); }

int getXPos() {
    uInt8 p = readDigitalU8(0);
    if (!getBitValue(p, 2)) return 1;
    if (!getBitValue(p, 1)) return 2;
    if (!getBitValue(p, 0)) return 3;
    return -1;
}

void gotoX(int x) {
    if (x < 1 || x > 3) return;

    // FIX 1A: se perdido (-1), vai para a posição 1 primeiro (referência)
    if (getXPos() == -1) {
        printf("[gotoX] posicao desconhecida, homing para X=1\n");
        ULONGLONG h = GetTickCount64();
        while (getXPos() != 1) {
            if (resetInProgress) { stopX(); return; }
            if (moveTimedOut(h, 5000, "gotoX-homing")) { stopX(); return; }
            moveXLeft(); vTaskDelay(1);
        }
        stopX();
    }

    int currentPos = getXPos();
    ULONGLONG start = GetTickCount64();

    if (x == 1) {
        while (getXPos() != 1) {
            if (resetInProgress) { stopX(); return; }
            if (moveTimedOut(start, 15000, "gotoX")) { stopX(); return; }
            moveXLeft(); vTaskDelay(1);
        }
        stopX();
    }
    else if (x == 2) {
        if (currentPos < 2) {
            while (getXPos() != 2) {
                if (resetInProgress) { stopX(); return; }
                if (moveTimedOut(start, 15000, "gotoX")) { stopX(); return; }
                moveXRight(); vTaskDelay(1);
            }
        }
        else {
            while (getXPos() != 2) {
                if (resetInProgress) { stopX(); return; }
                if (moveTimedOut(start, 15000, "gotoX")) { stopX(); return; }
                moveXLeft(); vTaskDelay(1);
            }
        }
        stopX();
    }
    else if (x == 3) {
        while (getXPos() != 3) {
            if (resetInProgress) { stopX(); return; }
            if (moveTimedOut(start, 15000, "gotoX")) { stopX(); return; }
            moveXRight(); vTaskDelay(1);
        }
        stopX();
    }
}

// ============================================================
// Y axis
// ============================================================
void moveYInside() { uInt8 p = readDigitalU8(2); setBitValue(&p, 5, 1); setBitValue(&p, 4, 0); writeDigitalU8(2, p); }
void moveYOutside() { uInt8 p = readDigitalU8(2); setBitValue(&p, 5, 0); setBitValue(&p, 4, 1); writeDigitalU8(2, p); }
void stopY() { uInt8 p = readDigitalU8(2); setBitValue(&p, 5, 0); setBitValue(&p, 4, 0); writeDigitalU8(2, p); }

int getYPos() {
    uInt8 p = readDigitalU8(0);
    if (!getBitValue(p, 5)) return 1;
    if (!getBitValue(p, 4)) return 2;
    if (!getBitValue(p, 3)) return 3;
    return -1;
}

void gotoY(int y) {
    if (y < 1 || y > 3) return;

    if (getYPos() == -1) {
        printf("[gotoY] posicao desconhecida, homing para Y=1\n");
        ULONGLONG h = GetTickCount64();
        while (getYPos() != 1) {
            if (resetInProgress) { stopY(); return; }
            if (moveTimedOut(h, 5000, "gotoY-homing")) { stopY(); return; }
            moveYOutside(); vTaskDelay(1);
        }
        stopY();
    }

    int currentPos = getYPos();
    ULONGLONG start = GetTickCount64();

    if (y == 1) {
        while (getYPos() != 1) {
            if (resetInProgress) { stopY(); return; }
            if (moveTimedOut(start, 15000, "gotoY")) { stopY(); return; }
            moveYOutside(); vTaskDelay(1);
        }
        stopY();
    }
    else if (y == 2) {
        if (currentPos < 2) {
            while (getYPos() != 2) {
                if (resetInProgress) { stopY(); return; }
                if (moveTimedOut(start, 15000, "gotoY")) { stopY(); return; }
                moveYInside(); vTaskDelay(1);
            }
        }
        else {
            while (getYPos() != 2) {
                if (resetInProgress) { stopY(); return; }
                if (moveTimedOut(start, 15000, "gotoY")) { stopY(); return; }
                moveYOutside(); vTaskDelay(1);
            }
        }
        stopY();
    }
    else if (y == 3) {
        while (getYPos() != 3) {
            if (resetInProgress) { stopY(); return; }
            if (moveTimedOut(start, 15000, "gotoY")) { stopY(); return; }
            moveYInside(); vTaskDelay(1);
        }
        stopY();
    }
}

// ============================================================
// Z axis
// ============================================================
void moveZUp() { uInt8 p = readDigitalU8(2); setBitValue(&p, 3, 1); setBitValue(&p, 2, 0); writeDigitalU8(2, p); }
void moveZDown() { uInt8 p = readDigitalU8(2); setBitValue(&p, 3, 0); setBitValue(&p, 2, 1); writeDigitalU8(2, p); }
void stopZ() { uInt8 p = readDigitalU8(2); setBitValue(&p, 3, 0); setBitValue(&p, 2, 0); writeDigitalU8(2, p); }

int getZPos() {
    uInt8 p0 = readDigitalU8(0);
    uInt8 p1 = readDigitalU8(1);
    if (!getBitValue(p1, 3)) return 1;
    if (!getBitValue(p1, 1)) return 2;
    if (!getBitValue(p0, 7)) return 3;
    if (!getBitValue(p1, 2)) return 4;
    if (!getBitValue(p1, 0)) return 5;
    if (!getBitValue(p0, 6)) return 6;
    return -1;
}

void gotoZ(int z) {
    if (z < 1 || z > 3) return;

    if (getZPos() == -1) {
        printf("[gotoZ] posicao desconhecida, homing para Z=1\n");
        ULONGLONG h = GetTickCount64();
        while (getZPos() != 1) {
            if (resetInProgress) { stopZ(); return; }
            if (moveTimedOut(h, 5000, "gotoZ-homing")) { stopZ(); return; }
            moveZDown(); vTaskDelay(1);
        }
        stopZ();
    }

    int currentPos = getZPos();
    ULONGLONG start = GetTickCount64();

    if (z == 1) {
        while (getZPos() != 1) {
            if (resetInProgress) { stopZ(); return; }
            if (moveTimedOut(start, 15000, "gotoZ")) { stopZ(); return; }
            moveZDown(); vTaskDelay(1);
        }
        stopZ();
    }
    else if (z == 2) {
        if (currentPos < 2) {
            while (getZPos() != 2) {
                if (resetInProgress) { stopZ(); return; }
                if (moveTimedOut(start, 15000, "gotoZ")) { stopZ(); return; }
                moveZUp(); vTaskDelay(1);
            }
        }
        else {
            while (getZPos() != 2) {
                if (resetInProgress) { stopZ(); return; }
                if (moveTimedOut(start, 15000, "gotoZ")) { stopZ(); return; }
                moveZDown(); vTaskDelay(1);
            }
        }
        stopZ();
    }
    else if (z == 3) {
        while (getZPos() != 3) {
            if (resetInProgress) { stopZ(); return; }
            if (moveTimedOut(start, 15000, "gotoZ")) { stopZ(); return; }
            moveZUp(); vTaskDelay(1);
        }
        stopZ();
    }
}

void gotoZUp() {
    int currentPos = getZPos();
    if (currentPos == -1) {
        ULONGLONG h = GetTickCount64();
        while (getZPos() != 1) {
            if (resetInProgress) { stopZ(); return; }
            if (moveTimedOut(h, 5000, "gotoZUp-homing")) { stopZ(); return; }
            moveZDown(); vTaskDelay(1);
        }
        stopZ();
        currentPos = 1;
    }

    int target = -1;
    if (currentPos == 1) target = 4;
    else if (currentPos == 2) target = 5;
    else if (currentPos == 3) target = 6;
    if (target == -1) return;

    ULONGLONG start = GetTickCount64();
    while (getZPos() != target) {
        if (resetInProgress) { stopZ(); return; }
        if (moveTimedOut(start, 15000, "gotoZUp")) { stopZ(); return; }
        moveZUp(); vTaskDelay(1);
    }
    stopZ();
}

void gotoZDown() {
    int currentPos = getZPos();
    if (currentPos == -1) {
        ULONGLONG h = GetTickCount64();
        while (getZPos() != 1) {
            if (resetInProgress) { stopZ(); return; }
            if (moveTimedOut(h, 5000, "gotoZDown-homing")) { stopZ(); return; }
            moveZDown(); vTaskDelay(1);
        }
        stopZ();
        currentPos = 1;
    }

    int target = -1;
    if (currentPos == 4) target = 1;
    else if (currentPos == 5) target = 2;
    else if (currentPos == 6) target = 3;
    if (target == -1) return;

    ULONGLONG start = GetTickCount64();
    while (getZPos() != target) {
        if (resetInProgress) { stopZ(); return; }
        if (moveTimedOut(start, 15000, "gotoZDown")) { stopZ(); return; }
        moveZDown(); vTaskDelay(1);
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
