#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"


#include "arkanoidEngeen.h"

#include "stdio.h"

#define MAX_INT (~(uint32_t)0 >> 1)
static const int32_t minMinus = ~(~(uint32_t)0 >> 1);

typedef struct ArkanoidDeskriptor {
    ElementDescr   *elementList;
    int32_t        platformHeight;
    int32_t        platformWidth;
    int32_t        areaHeight;
    int32_t        areaWidth;
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
        int32_t  x;
        int32_t  y;
        int32_t  dx;
        int32_t  dy;
        bool     isVertical;
        int32_t  dist;
        uint16_t cnt;
        uint16_t list[10];
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
    if(z11 != 0) {
        z11 = (z11 > 0) ? (1) : (-1);
    }
    int32_t z12 = v1.x * v12.y - v1.y * v12.x;
    if(z12 != 0) {
        z12 = (z12 > 0) ? (1) : (-1);
    }
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
    if(z21 != 0) {
        z21 = (z21 > 0) ? (1) : (-1);
    }
    int32_t z22 = v2.x * v22.y - v2.y * v22.x;
    if(z22 != 0) {
        z22 = (z22 > 0) ? (1) : (-1);
    }
    //printf("z22  = %d\n", z22);
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

ArkanoidStatus arkanoidSetDirection(ArkanoidH arkanoidItem, int32_t dirLen, int32_t dirX, int32_t dirY) {
    arkanoidItem->dirX  = dirX;
    arkanoidItem->dirY  = dirY;
    arkanoidItem->dirLength = dirLen;
}

static inline bool analyseCross(ArkanoidH arkanoidItem, int32_t AL, int32_t BL, int32_t CL,
                         int32_t segment1_X1, int32_t segment1_X2, int32_t segment1_Y1, int32_t segment1_Y2,
                         int32_t segment2_X1, int32_t segment2_X2, int32_t segment2_Y1, int32_t segment2_Y2)
{
    if(!isCrose(segment1_X1, segment1_X2, segment1_Y1, segment1_Y2,
               segment2_X1, segment2_X2, segment2_Y1, segment2_Y2)) {
        return false;
    }
    int32_t crosDist;
    bool isVertical = false;
    if(segment2_X1 == segment2_X2) {
        isVertical = true;
        arkanoidItem->cross.x = segment2_X1;
        arkanoidItem->cross.y = (BL * arkanoidItem->cross.x + CL) / AL;
    } else {
        arkanoidItem->cross.y = segment2_Y1;
        arkanoidItem->cross.x = (AL * arkanoidItem->cross.y - CL) / BL;
    }
    arkanoidItem->cross.dx = arkanoidItem->cross.x - segment1_X1;
    arkanoidItem->cross.dy = arkanoidItem->cross.y - segment1_Y1;

    if(!((crosDist = (arkanoidItem->cross.dx * arkanoidItem->cross.dx)
                   + (arkanoidItem->cross.dy * arkanoidItem->cross.dy)) < arkanoidItem->cross.dist)) {
       return false;
    }
    if(arkanoidItem->cross.dist ==  crosDist) {
        /*TODO increase counter of cross.... realy ??*/
        return false;
    }
    arkanoidItem->cross.dist = crosDist;
    arkanoidItem->cross.isVertical = isVertical;
    arkanoidItem->cross.cnt = 1;
    //arkanoidItem->cross.index[0] = k;
    return true;
}

ArkanoidStatus arkanoidUpdate(ArkanoidH arkanoidItem, uint32_t currentTime)
{
    int32_t time;
    if( (time = currentTime - arkanoidItem->time) <= 0 ) {
        return arkanoidItem->status = STATUS_ERRO;
    }
    arkanoidItem->time = currentTime;
    int32_t currentDist = arkanoidItem->ballSpeed * time;
    int32_t currentX    = (currentDist * arkanoidItem->dirX) / arkanoidItem->dirLength + arkanoidItem->ballPosX;
    int32_t currentY    = (currentDist * arkanoidItem->dirY) / arkanoidItem->dirLength + arkanoidItem->ballPosY;
    int32_t AL, BL, CL;
    arkanoidItem->cross.cnt = 0;
    while(true) {
        arkanoidItem->cross.dist = MAX_INT;
        AL = arkanoidItem->ballPosX - currentX;
        BL = arkanoidItem->ballPosY - currentY;

        //CL = (currentDist * arkanoidItem->dirY * arkanoidItem->ballPosX) / arkanoidItem->dirLength + arkanoidItem->ballPosX * arkanoidItem->ballPosY -
             ((currentDist * arkanoidItem->dirX * arkanoidItem->ballPosY) / arkanoidItem->dirLength + arkanoidItem->ballPosY * arkanoidItem->ballPosX);

        CL = arkanoidItem->ballPosX * currentY - arkanoidItem->ballPosY * currentX;

        // Cross with objects
        for(uint16_t k = 0; k < arkanoidItem->elementQuantity; k++) {
            if(!arkanoidItem->elementList[k].isPresent) { // if element was deleted
                continue;
            }
            // twoo vertical side
            if(analyseCross(arkanoidItem, AL, BL, CL,
                         arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                         arkanoidItem->elementList[k].x1, arkanoidItem->elementList[k].x1,
                         arkanoidItem->elementList[k].y1, arkanoidItem->elementList[k].y2)) {
                arkanoidItem->cross.list[0] = k;
            };

            if(analyseCross(arkanoidItem, AL, BL, CL,
                         arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                         arkanoidItem->elementList[k].x2, arkanoidItem->elementList[k].x2,
                         arkanoidItem->elementList[k].y1, arkanoidItem->elementList[k].y2)) {
                arkanoidItem->cross.list[0] = k;
            };
            // twoo horizontal side
            if(analyseCross(arkanoidItem, AL, BL, CL,
                         arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                         arkanoidItem->elementList[k].x1, arkanoidItem->elementList[k].x2,
                         arkanoidItem->elementList[k].y1, arkanoidItem->elementList[k].y1)) {
                arkanoidItem->cross.list[0] = k;
            };

            if(analyseCross(arkanoidItem, AL, BL, CL,
                         arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                         arkanoidItem->elementList[k].x1, arkanoidItem->elementList[k].x2,
                         arkanoidItem->elementList[k].y2, arkanoidItem->elementList[k].y2)) {
                arkanoidItem->cross.list[0] = k;
            };
        }
        /***********Cross with borders*************************************************/
        // twoo vertical borders
        if(analyseCross(arkanoidItem, AL, BL, CL,
                     arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                     0, 0,
                     0, arkanoidItem->areaHeight)) {
            arkanoidItem->cross.cnt = 0;
        };

        if(analyseCross(arkanoidItem, AL, BL, CL,
                     arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                     arkanoidItem->areaWidth, arkanoidItem->areaWidth,
                     0, arkanoidItem->areaHeight)) {
            arkanoidItem->cross.cnt = 0;
        };
        // UP horizontal border
        if(analyseCross(arkanoidItem, AL, BL, CL,
                     arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                     0, arkanoidItem->areaWidth,
                     0, 0)) {
            arkanoidItem->cross.cnt = 0;
        };
        /*
        //ONLY FOR TEST: DOWN horizontal border
        if(analyseCross(arkanoidItem, AL, BL, CL,
                     arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                     0, arkanoidItem->areaWidth,
                     arkanoidItem->areaHeight, arkanoidItem->areaHeight)) {
            arkanoidItem->cross.cnt = 0;
        };
*/
        /***********Cross with platform******************/
        //platform UP border
        if(analyseCross(arkanoidItem, AL, BL, CL,
                     arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                     arkanoidItem->platformPosX, arkanoidItem->platformPosX + arkanoidItem->platformWidth,
                     arkanoidItem->platformPosY, arkanoidItem->platformPosY)) {
            arkanoidItem->cross.cnt = 0;
        };

        // if no cross
        if((arkanoidItem->cross.dist == MAX_INT) || (arkanoidItem->cross.dist == 0)){
            arkanoidItem->ballPosX = currentX;
            arkanoidItem->ballPosY = currentY;
            break;
        }
        if(arkanoidItem->cross.isVertical) { // cross with vertical element
            arkanoidItem->dirX *= -1;
        } else {         // cross with horizontal element
            arkanoidItem->dirY *= -1;
        }
        int32_t dist = (arkanoidItem->cross.dx) ?
                       (currentDist * (arkanoidItem->ballPosX - currentX)) / arkanoidItem->cross.dx :
                       (currentDist * (arkanoidItem->ballPosY - currentY)) / arkanoidItem->cross.dy;
        if(dist < 0) {
            dist = (-1) * dist;
        }
        printf("dist = %d\n", dist);
        printf("crosX = %d\n", arkanoidItem->cross.x);
        printf("crosY = %d\n", arkanoidItem->cross.y);
        if(dist != 0) {
            arkanoidItem->ballPosX = (dist * arkanoidItem->dirX) / arkanoidItem->dirLength + arkanoidItem->cross.x;
            arkanoidItem->ballPosY = (dist * arkanoidItem->dirY) / arkanoidItem->dirLength + arkanoidItem->cross.y;
        } else {
            arkanoidItem->ballPosX = currentX;
            arkanoidItem->ballPosY = currentY;
        }
        // if cross with obj
        if(arkanoidItem->cross.cnt != 0) {
            arkanoidItem->elementList[arkanoidItem->cross.list[0]].isPresent = false;
        }
        break;
    }

    //printf("currentX = %d\n", currentX);
    //printf("currentY = %d\n", currentY);
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
