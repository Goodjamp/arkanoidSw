#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"
#include "math.h"

#include "arkanoidEngeen.h"

#include "stdio.h"

typedef struct ArkanoidDeskriptor {
    ElementDescr   *elementList;
    int32_t       platformHeight;
    int32_t       platformWidth;
    int32_t       areaHeight;
    int32_t       areaWidth;
    int32_t        ballSpeed;
    ArkanoidStatus status;
    int32_t        platformPosX;
    int32_t        platformPosY;
    int32_t        ballPosX;
    int32_t        ballPosY;
    int32_t        dirX;
    int32_t        dirY;
    int32_t        dirLength;
    uint32_t       time;
    uint16_t       elementQuantity;
    struct {
        uint16_t cnt;
        uint16_t index[40];
    }cross;
} ArkanoidDeskriptor;

static uint8_t itemCount;
static ArkanoidDeskriptor arkanoidExample[ARKANOID_ITEMS_QUANTITY];

static inline bool isCrose(int32_t x11, int32_t x21, int32_t y11, int32_t y21,
                           int32_t x12, int32_t x22, int32_t y12, int32_t y22)
{
    struct Vector {
        int32_t x;
        int32_t y;
    };
    // line 2 cross line 1
    struct Vector  v1 = {
        .x = x21 - x11,
        .y = y21 - y11,
    };
    struct Vector  v11 = {
        .x = x12 - x11,
        .y = y12 - y11,
    };
    struct Vector  v12 = {
        .x = x22 - x11,
        .y = y22 - y11,
    };
    int32_t z11 = v1.x * v11.y - v1.y * v11.x;
    int32_t z12 = v1.x * v12.y - v1.y * v12.x;
    if((z11 * z12) > 0) {
        return false;
    }
    // line 1 cross line 2
    struct Vector  v2 = {
        .x = x22 - x12,
        .y = y22 - y12,
    };
    struct Vector  v21 = {
        .x = x11 - x12,
        .y = y11 - y12,
    };
    struct Vector  v22 = {
        .x = x21 - x12,
        .y = y21 - y12,
    };
    int32_t z21 = v2.x * v21.y - v2.y * v21.x;
    int32_t z22 = v2.x * v22.y - v2.y * v22.x;
    printf("z22  = %d\n", z22);
    if((z21 * z22) > 0) {
        return false;
    }
    return true;
}


ArkanoidH arkanoidInit(ArkanoidConfig arkanoidConfig)
{
    if((itemCount) >= ARKANOID_ITEMS_QUANTITY) {
        return NULL;
    }
    arkanoidExample[itemCount].elementList     = arkanoidConfig.elementList;
    arkanoidExample[itemCount].elementQuantity = arkanoidConfig.elementQuantity;
    arkanoidExample[itemCount].platformPosX    = arkanoidConfig.platformPosX;
    arkanoidExample[itemCount].platformPosY    = arkanoidConfig.platformPosY;
    arkanoidExample[itemCount].platformHeight  = arkanoidConfig.platformWidth;
    arkanoidExample[itemCount].platformWidth   = arkanoidConfig.platformWidth;
    arkanoidExample[itemCount].areaHeight      = arkanoidConfig.areaHeight;
    arkanoidExample[itemCount].areaWidth       = arkanoidConfig.areaWidth;
    arkanoidExample[itemCount].ballSpeed       = arkanoidConfig.ballSpeed;
    arkanoidSetDirection(&arkanoidExample[itemCount], arkanoidConfig.dirX, arkanoidConfig.dirY);

    for(uint32_t k = 0; k < arkanoidExample[itemCount].elementQuantity; k++) {
        arkanoidExample[itemCount].elementList[k].isPresent = true;
    }
    itemCount++;
    return &arkanoidExample[itemCount - 1];
}

bool arkanoidGetElementState(ArkanoidH arkanoidItem, uint32_t elemenPos)
{
    return arkanoidItem->elementList[elemenPos].isPresent;
}

ArkanoidStatus arkanoidSetSpeed(ArkanoidH arkanoidItem, uint8_t ballSpeed)
{
    arkanoidItem->ballSpeed = ballSpeed;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidSetDirection(ArkanoidH arkanoidItem, int32_t dirX, int32_t dirY) {
    arkanoidItem->dirX            = dirX;
    arkanoidItem->dirY            = dirY;
    arkanoidItem->dirLength       = sqrt(arkanoidItem->dirX * arkanoidItem->dirX
                                         + arkanoidItem->dirY * arkanoidItem->dirY);

}

static inline void lineCrossPoint(int32_t x11, int32_t x21, int32_t y11, int32_t y21,
                                  int32_t x12, int32_t x22, int32_t y12, int32_t y22)
{

}


ArkanoidStatus arkanoidUpdate(ArkanoidH arkanoidItem, uint32_t currentTime)
{
    int32_t time;
    if( (time = currentTime - arkanoidItem->time) <= 0 ) {
        return arkanoidItem->status = STATUS_ERRO;
    }
    int32_t currentDist = arkanoidItem->ballSpeed * time;
    int32_t currentX    = (currentDist * arkanoidItem->dirX) / arkanoidItem->dirLength + arkanoidItem->ballPosX;
    int32_t currentY    = (currentDist * arkanoidItem->dirY) / arkanoidItem->dirLength + arkanoidItem->ballPosY;
    int32_t minDist;
    int32_t crosX;
    int32_t crosY;
    int32_t crosDist;
    int32_t AL, BL, CL;
    arkanoidItem->cross.cnt = 0;
    bool isVertical;
    while(true) {
        minDist = -1;
        AL = arkanoidItem->ballPosX - currentX;
        BL = arkanoidItem->ballPosY - currentY;
        CL = arkanoidItem->ballPosY * currentX - arkanoidItem->ballPosX * currentY;

        for(uint16_t k = 0; k < arkanoidItem->elementQuantity; k++) {
            // twoo vertical side
            if(isCrose(arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                       arkanoidItem->elementList[k].x1, arkanoidItem->elementList[k].x1,
                       arkanoidItem->elementList[k].y1, arkanoidItem->elementList[k].y2)) {
                crosX = arkanoidItem->elementList[k].x1;
                crosY = (BL * crosX + CL) / AL;
                if((crosDist = (crosX - arkanoidItem->ballPosX) * (crosX - arkanoidItem->ballPosX)
                               + (crosX - arkanoidItem->ballPosY) * (crosX - arkanoidItem->ballPosY)) < minDist) {
                    minDist = crosDist;
                    isVertical = true;
                    arkanoidItem->cross.cnt = 1;
                } else if(crosDist ==  minDist) {

                }
            }
            if(isCrose(arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                       arkanoidItem->elementList[k].x2, arkanoidItem->elementList[k].x2,
                       arkanoidItem->elementList[k].y1, arkanoidItem->elementList[k].y2)) {
                crosX = arkanoidItem->elementList[k].x2;
                crosY = (BL * crosX + CL) / AL;
                if((crosDist = (crosX - arkanoidItem->ballPosX) * (crosX - arkanoidItem->ballPosX)
                               + (crosX - arkanoidItem->ballPosY) * (crosX - arkanoidItem->ballPosY)) < minDist) {
                    minDist = crosDist;
                    isVertical = true;
                    arkanoidItem->cross.cnt = 1;
                } else if(crosDist ==  minDist) {

                }
            }
            // twoo horizontal side
            if(isCrose(arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                       arkanoidItem->elementList[k].x1, arkanoidItem->elementList[k].x2,
                       arkanoidItem->elementList[k].y1, arkanoidItem->elementList[k].y1)) {
                crosY = arkanoidItem->elementList[k].y1;
                crosX = (AL * crosY - CL) / BL;
                if((crosDist = (crosX - arkanoidItem->ballPosX) * (crosX - arkanoidItem->ballPosX)
                               + (crosX - arkanoidItem->ballPosY) * (crosX - arkanoidItem->ballPosY)) < minDist) {
                    minDist = crosDist;
                    isVertical = false;
                    arkanoidItem->cross.cnt = 1;
                } else if(crosDist ==  minDist) {

                }
            }
            if(isCrose(arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                       arkanoidItem->elementList[k].x1, arkanoidItem->elementList[k].x2,
                       arkanoidItem->elementList[k].y2, arkanoidItem->elementList[k].y2)) {
                crosY = arkanoidItem->elementList[k].y2;
                crosX = (AL * crosY - CL) / BL;
                if((crosDist = (crosX - arkanoidItem->ballPosX) * (crosX - arkanoidItem->ballPosX)
                               + (crosX - arkanoidItem->ballPosY) * (crosX - arkanoidItem->ballPosY)) < minDist) {
                    minDist = crosDist;
                    isVertical = false;
                    arkanoidItem->cross.cnt = 1;
                } else if(crosDist ==  minDist) {

                }
            }

        }
        // if no cross
        if(minDist < 0 ){
            break;
        }
        if(isVertical) {

        }

    }
    arkanoidItem->ballPosX = currentX;
    arkanoidItem->ballPosY = currentY;
    /*
    static uint16_t CNT = 0;
    float arg = 2.f * M_PI * 1.f * time / 1000.f;
    arkanoidItem->ballPosX = arkanoidItem->areaWidth / 2 + 50.f * cosf(arg);
    arkanoidItem->ballPosY = arkanoidItem->areaHeight /2 + 50.f * sinf(arg);
    for(uint16_t k = 0; k < CNT; k++) {
        arkanoidItem->elementList[k].isPresent = false;
    }
    CNT++;
    if(CNT >= arkanoidItem->elementQuantity) {
        for(uint16_t k = 0; k < arkanoidItem->elementQuantity; k++) {
            arkanoidItem->elementList[k].isPresent = true;
        }
        CNT = 0;
    }
    */
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidSetPlatformSize(ArkanoidH arkanoidItem, int32_t width, int32_t height)
{
    arkanoidItem->platformWidth = width;
    arkanoidItem->platformHeight = height;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidSetPlatformPos(ArkanoidH arkanoidItem, int32_t x, int32_t y)
{
    arkanoidItem->platformPosX = x;
    arkanoidItem->platformPosY = y;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidSetBallPos(ArkanoidH arkanoidItem, int32_t x, int32_t y)
{
    arkanoidItem->ballPosX = x;
    arkanoidItem->ballPosY = y;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidGetBallPos(ArkanoidH arkanoidItem, int32_t *x, int32_t *y)
{
    *x = arkanoidItem->ballPosX;
    *y = arkanoidItem->ballPosY;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidGetStatus(ArkanoidH arkanoidItem)
{
    return arkanoidItem->status;
}
