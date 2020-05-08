#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"


#include "arkanoidEngeen.h"

#include "stdio.h"

#define MAX_INT (~(uint32_t)0 >> 1)

typedef struct ArkanoidDeskriptor {
    ElementDescr   *elementList;
    uint16_t       elementQuantity;
    int32_t        platformHeight;
    int32_t        platformWidth;
    ElementDescr   borders;
    ElementDescr   platform;
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
    int32_t        dir2X;
    int32_t        dir2Y;
    int32_t        crossX;
    int32_t        crossY;
    uint32_t       time;

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

    bool     isXLine;
    int32_t  dirPointX; // direction for point X
    int32_t  dirPointY; // direction for point Y
    uint32_t pointsPerFrame;
} ArkanoidDeskriptor;

static uint8_t itemCount;
static ArkanoidDeskriptor arkanoidExample[ARKANOID_ITEMS_QUANTITY];

static inline void arkanoidVerticalReflectionUpdate(ArkanoidH arkanoidItem)
{
    arkanoidItem->dirX      *= (-1);
    arkanoidItem->dir2X     = 2 * arkanoidItem->dirX;
    arkanoidItem->dirPointX *= (-1);
}

static inline void arkanoidHorizontalReflectionUpdate(ArkanoidH arkanoidItem)
{
    arkanoidItem->dirY      *= (-1);
    arkanoidItem->dir2Y     = 2 * arkanoidItem->dirY;
    arkanoidItem->dirPointY *= (-1);
}

static inline void arkanoidEngleReflectionUpdate(ArkanoidH arkanoidItem)
{
   arkanoidVerticalReflectionUpdate(arkanoidItem);
   arkanoidHorizontalReflectionUpdate(arkanoidItem);
}

ArkanoidStatus arkanoidUpdate(ArkanoidH arkanoidItem, uint32_t currentTime)
{
    if (arkanoidItem->status != STATUS_GAME) {
        return arkanoidItem->status;
    }
    int32_t x = arkanoidItem->ballPosX;
    int32_t y = arkanoidItem->ballPosY;
    bool crossElement = false;
    for(uint32_t k = 0; k < arkanoidItem->pointsPerFrame; k++) {
        if(arkanoidItem->isXLine) {
            x += arkanoidItem->dirPointX;
            /*Calculate next Y */
            y = (arkanoidItem->dir2Y *(x - arkanoidItem->crossX) + arkanoidItem->dirX) / arkanoidItem->dir2X
                    + arkanoidItem->crossY;
        } else { // Y line direction
            y += arkanoidItem->dirPointY;
            /*Calculate next X */
            x = (arkanoidItem->dir2X *(y - arkanoidItem->crossY) + arkanoidItem->dirY) / arkanoidItem->dir2Y
                    + arkanoidItem->crossX;
        }
        crossElement = false;
        /*Cross with elements*/
        for(uint16_t m = 0; m < arkanoidItem->elementQuantity; m++) {
            if(!arkanoidItem->elementList[m].isPresent) { // if element was deleted
                continue;
            }
            if((x == arkanoidItem->elementList[m].x1 || x == arkanoidItem->elementList[m].x2)
               && (y > arkanoidItem->elementList[m].y1) && (y < arkanoidItem->elementList[m].y2)) {
                arkanoidVerticalReflectionUpdate(arkanoidItem);
            } else if ((y == arkanoidItem->elementList[m].y1 || y == arkanoidItem->elementList[m].y2)
                       && (x > arkanoidItem->elementList[m].x1) && (x < arkanoidItem->elementList[m].x2)){
                arkanoidHorizontalReflectionUpdate(arkanoidItem);
            } else if ((x == arkanoidItem->elementList[m].x1 || x == arkanoidItem->elementList[m].x2)
                       && (y == arkanoidItem->elementList[m].y1 || y == arkanoidItem->elementList[m].y2)){
                arkanoidEngleReflectionUpdate(arkanoidItem);
            } else {
                continue;
            }
            arkanoidItem->elementList[m].isPresent = false;
            arkanoidItem->crossX = x;
            arkanoidItem->crossY = y;
            crossElement = true;
            break;
        }
        if(crossElement) {
            break;
        }

        /*Cross with game space borders*/
        if ((x == arkanoidItem->borders.x1 || x == arkanoidItem->borders.x2)
            && ((y > arkanoidItem->borders.y1) && (y < arkanoidItem->borders.y2))) {
            arkanoidVerticalReflectionUpdate(arkanoidItem);
            x += (x == arkanoidItem->borders.x1) ? ( 1 ) : (-1);
            crossElement = true;
        } else if ((y == arkanoidItem->borders.y1)
                   &&(x > arkanoidItem->borders.x1) && (x < arkanoidItem->borders.x2)){
            arkanoidHorizontalReflectionUpdate(arkanoidItem);
            y += 1;
            crossElement = true;
        } else if ((y == arkanoidItem->borders.y1)
                   && ((x == arkanoidItem->borders.x1) || (x == arkanoidItem->borders.x2))) {
            arkanoidEngleReflectionUpdate(arkanoidItem);
            crossElement = true;
        } else if (y == arkanoidItem->borders.y2) {

            return arkanoidItem->status = STATUS_CRASH;               //GAME OVER !!!! YOU LOOSER !!!

        }
        if(crossElement) {
            arkanoidItem->crossX = x;
            arkanoidItem->crossY = y;
            continue;
        }

        /*Cross with platform*/
        if((y == arkanoidItem->platform.y1)
           && (x >= arkanoidItem->platform.x1) && (x <= arkanoidItem->platform.x2)) {
            arkanoidHorizontalReflectionUpdate(arkanoidItem);
            y -= 1;
            crossElement = true;
        }
        if(crossElement) {
            arkanoidItem->crossX = x;
            arkanoidItem->crossY = y;
            continue;
        }
    }
    arkanoidItem->ballPosX = x;
    arkanoidItem->ballPosY = y;

    return arkanoidItem->status;
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
    arkanoidExample[itemCount].platformHeight  = arkanoidConfig.platformHeight;
    arkanoidExample[itemCount].platformWidth   = arkanoidConfig.platformWidth;
    arkanoidExample[itemCount].areaHeight      = arkanoidConfig.areaHeight;
    arkanoidExample[itemCount].areaWidth       = arkanoidConfig.areaWidth;
    arkanoidExample[itemCount].ballSpeed       = arkanoidConfig.ballSpeed;
    arkanoidExample[itemCount].pointsPerFrame  = arkanoidConfig.pointsPerFrame;

    arkanoidExample[itemCount].borders.x1 = 0;
    arkanoidExample[itemCount].borders.x2 = arkanoidConfig.areaWidth - 1;
    arkanoidExample[itemCount].borders.y1 = 0;
    arkanoidExample[itemCount].borders.y2 = arkanoidConfig.areaHeight - 1;

    arkanoidSetPlatformPos(&arkanoidExample[itemCount], arkanoidConfig.platformPosX, arkanoidConfig.platformPosY);


    for(uint32_t k = 0; k < arkanoidExample[itemCount].elementQuantity; k++) {
        arkanoidExample[itemCount].elementList[k].isPresent = true;
    }
    itemCount++;
    arkanoidExample[itemCount].status = STATUS_GAME;
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

static inline void arkanoidDirectionProcessing(ArkanoidH arkanoidItem)
{

    uint32_t dXLen = (arkanoidItem->dirX >= 0) ? (arkanoidItem->dirX) : (arkanoidItem->dirX * (-1));
    uint32_t dYLen = (arkanoidItem->dirY >= 0) ? (arkanoidItem->dirY) : (arkanoidItem->dirY * (-1));
    arkanoidItem->dir2X = 2 * arkanoidItem->dirX;
    arkanoidItem->dir2Y = 2 * arkanoidItem->dirY;
    arkanoidItem->dirPointX = (arkanoidItem->dirX >= 0) ? (1) : (-1);
    arkanoidItem->dirPointY = (arkanoidItem->dirY >= 0) ? (1) : (-1);
    arkanoidItem->isXLine = (dXLen >= dYLen);

}

ArkanoidStatus arkanoidSetDirection(ArkanoidH arkanoidItem, int32_t dirLen, int32_t dirX, int32_t dirY) {
    arkanoidItem->dirX  = dirX;
    arkanoidItem->dirY  = dirY;
    arkanoidItem->dirLength = dirLen;
    arkanoidDirectionProcessing(arkanoidItem);
}

static inline bool isCrose(int32_t x11, int32_t x21, int32_t y11, int32_t y21,
                           int32_t x12, int32_t x22, int32_t y12, int32_t y22)
{
/*
(x11 ; y11) - begin ball position
(x21 ; y21) - end ball position
*/

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

static inline bool isCrose_(int32_t x11, int32_t x21, int32_t y11, int32_t y21,
                           int32_t x12, int32_t x22, int32_t y12, int32_t y22)
{
/*
(x11 ; y11) - begin ball position
(x21 ; y21) - end ball position
*/
    struct Vector {
        int32_t x;
        int32_t y;
    };
    struct Vector  v11 = {
        .x = x12 - x11,
        .y = y12 - y11,
    };
    struct Vector  v12 = {
        .x = x22 - x11,
        .y = y22 - y11,
    };
    struct Vector  v21 = {
        .x = x12 - x21,
        .y = y12 - y21,
    };
    struct Vector  v22 = {
        .x = x22 - x21,
        .y = y22 - y21,
    };

    int32_t z1 = v11.x * v12.y - v11.y * v12.x;
    int32_t z2 = v21.x * v22.y - v21.y * v22.x;
    if(z1 == 0) {
        return false;
    }
    if(z2 == 0) {
        // calc scalar
        return (v21.x * v22.x + v21.y * v22.y) <= 0;
    }
    if ((z1 > 0 && z2 > 0)
        || (z1 < 0 && z2 < 0)) {
        return false;
    }
    return true;
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
    crosDist = (arkanoidItem->cross.dx * arkanoidItem->cross.dx)
               + (arkanoidItem->cross.dy * arkanoidItem->cross.dy);
    if( crosDist > arkanoidItem->cross.dist) {
       return false;
    } else if (crosDist == arkanoidItem->cross.dist ) {
        return true;
    } else {
        arkanoidItem->cross.dist = crosDist;
        arkanoidItem->cross.isVertical = isVertical;
        arkanoidItem->cross.cnt = 0;
        return true;
    }
}

ArkanoidStatus arkanoidUpdate_(ArkanoidH arkanoidItem, uint32_t currentTime)
{
    int32_t time;
    if( (time = currentTime - arkanoidItem->time) <= 0 ) {
        return arkanoidItem->status = STATUS_ERRO;
    }
    arkanoidItem->time = currentTime;
    int32_t currentDist = arkanoidItem->ballSpeed * time;
    int32_t AL, BL, CL;
    int32_t currentX;
    int32_t currentY;
    arkanoidItem->cross.cnt = 0;
    while(true) {
        currentX    = (currentDist * arkanoidItem->dirX) / arkanoidItem->dirLength + arkanoidItem->ballPosX;
        currentY    = (currentDist * arkanoidItem->dirY) / arkanoidItem->dirLength + arkanoidItem->ballPosY;
        arkanoidItem->cross.dist = MAX_INT;
        AL = arkanoidItem->ballPosX - currentX;
        BL = arkanoidItem->ballPosY - currentY;

        //CL = (currentDist * arkanoidItem->dirY * arkanoidItem->ballPosX) / arkanoidItem->dirLength + arkanoidItem->ballPosX * arkanoidItem->ballPosY -
        //     ((currentDist * arkanoidItem->dirX * arkanoidItem->ballPosY) / arkanoidItem->dirLength + arkanoidItem->ballPosY * arkanoidItem->ballPosX);

        CL = arkanoidItem->ballPosX * currentY - arkanoidItem->ballPosY * currentX;
        bool isElementCross = false;
        // Cross with objects
        for(uint16_t k = 0; k < arkanoidItem->elementQuantity; k++) {
            if(!arkanoidItem->elementList[k].isPresent) { // if element was deleted
                continue;
            }
            isElementCross = false;
            // twoo vertical side
            if(analyseCross(arkanoidItem, AL, BL, CL,
                         arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                         arkanoidItem->elementList[k].x1, arkanoidItem->elementList[k].x1,
                         arkanoidItem->elementList[k].y1, arkanoidItem->elementList[k].y2)) {
                isElementCross = true;
            };

            if(analyseCross(arkanoidItem, AL, BL, CL,
                         arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                         arkanoidItem->elementList[k].x2, arkanoidItem->elementList[k].x2,
                         arkanoidItem->elementList[k].y1, arkanoidItem->elementList[k].y2)) {
                isElementCross = true;
            };
            // twoo horizontal side
            if(analyseCross(arkanoidItem, AL, BL, CL,
                         arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                         arkanoidItem->elementList[k].x1, arkanoidItem->elementList[k].x2,
                         arkanoidItem->elementList[k].y1, arkanoidItem->elementList[k].y1)) {
                isElementCross = true;
            };

            if(analyseCross(arkanoidItem, AL, BL, CL,
                         arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                         arkanoidItem->elementList[k].x1, arkanoidItem->elementList[k].x2,
                         arkanoidItem->elementList[k].y2, arkanoidItem->elementList[k].y2)) {
                isElementCross = true;
            };
            if(isElementCross) {
                arkanoidItem->cross.list[arkanoidItem->cross.cnt++] = k;
            }
        }
        /***********Cross with borders*************************************************/
        // twoo vertical borders
        if(analyseCross(arkanoidItem, AL, BL, CL,
                     arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                     0, 0,
                     0, arkanoidItem->areaHeight)) {
            isElementCross = true;
        };

        if(analyseCross(arkanoidItem, AL, BL, CL,
                     arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                     arkanoidItem->areaWidth, arkanoidItem->areaWidth,
                     0, arkanoidItem->areaHeight)) {
            isElementCross = true;
        };
        // UP horizontal border
        if(analyseCross(arkanoidItem, AL, BL, CL,
                     arkanoidItem->ballPosX, currentX, arkanoidItem->ballPosY, currentY,
                     0, arkanoidItem->areaWidth,
                     0, 0)) {
            isElementCross = true;
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
            isElementCross = true;
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
        // distance to cross point
        int32_t dist = (arkanoidItem->cross.dx) ?
                       (currentDist * arkanoidItem->cross.dx) /  (arkanoidItem->ballPosX - currentX):
                       (currentDist * arkanoidItem->cross.dy) / (arkanoidItem->ballPosY - currentY);
        if(dist < 0) {
            currentDist += dist;
        } else {
            currentDist -= dist;
        }
        if(dist != 0) {
            arkanoidItem->ballPosX = arkanoidItem->cross.x;//(currentDist * arkanoidItem->dirX) / arkanoidItem->dirLength + arkanoidItem->cross.x;
            arkanoidItem->ballPosY = arkanoidItem->cross.y;//(currentDist * arkanoidItem->dirY) / arkanoidItem->dirLength + arkanoidItem->cross.y;
        } else {
            arkanoidItem->ballPosX = currentX;
            arkanoidItem->ballPosY = currentY;
        }
        // if cross with obj
        if(arkanoidItem->cross.cnt != 0) {
            arkanoidItem->elementList[arkanoidItem->cross.list[0]].isPresent = false;
        }
        if(currentDist <= 0) {
            break;
        }
    }
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
    arkanoidItem->platform.x1 = x;
    arkanoidItem->platform.x2 = x + arkanoidItem->platformWidth - 1;
    arkanoidItem->platform.y1 = y;
    arkanoidItem->platform.y2 = y + arkanoidItem->platformHeight - 1;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidSetBallPos(ArkanoidH arkanoidItem, int32_t x, int32_t y)
{
    arkanoidItem->ballPosX = x;
    arkanoidItem->ballPosY = y;
    arkanoidItem->crossX   = x;
    arkanoidItem->crossY   = y;
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
