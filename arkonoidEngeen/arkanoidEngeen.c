#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

#include "arkanoidEngeen.h"

#include "stdio.h"

typedef struct ArkanoidDeskriptor {
    ElementDescr   *elementList;
    uint16_t       elementQuantity;
    int32_t        platformHeight;
    int32_t        platformWidth;
    ElementDescr   borders;
    ElementDescr   platform;
    int32_t        ballPosX;
    int32_t        ballPosY;
    int32_t        dirX;
    int32_t        dirY;
    int32_t        dir2X;
    int32_t        dir2Y;
    int32_t        crossX;
    int32_t        crossY;
    int32_t        dirPointX; // direction for point X
    int32_t        dirPointY; // direction for point Y
    bool           isXLine;
    uint32_t       pointsPerFrame;
    ArkanoidStatus status;
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

ArkanoidH arkanoidInit(ArkanoidConfig arkanoidConfig)
{
    if((itemCount) >= ARKANOID_ITEMS_QUANTITY) {
        return NULL;
    }
    arkanoidExample[itemCount].elementList     = arkanoidConfig.elementList;
    arkanoidExample[itemCount].elementQuantity = arkanoidConfig.elementQuantity;
    arkanoidExample[itemCount].platformHeight  = arkanoidConfig.platformHeight;
    arkanoidExample[itemCount].platformWidth   = arkanoidConfig.platformWidth;
    arkanoidExample[itemCount].pointsPerFrame  = arkanoidConfig.pointsPerFrame;
    arkanoidExample[itemCount].borders.x1 = 0;
    arkanoidExample[itemCount].borders.x2 = arkanoidConfig.areaWidth - 1;
    arkanoidExample[itemCount].borders.y1 = 0;
    arkanoidExample[itemCount].borders.y2 = arkanoidConfig.areaHeight - 1;
    for(uint32_t k = 0; k < arkanoidExample[itemCount].elementQuantity; k++) {
        arkanoidExample[itemCount].elementList[k].isPresent = true;
    }
    arkanoidExample[itemCount].status = STATUS_GAME;
    arkanoidSetPlatformPos(&arkanoidExample[itemCount], arkanoidConfig.platformPosX, arkanoidConfig.platformPosY);
    itemCount++;
    return &arkanoidExample[itemCount - 1];
}

ArkanoidStatus arkanoidUpdate(ArkanoidH arkanoidItem)
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

bool arkanoidGetElementState(ArkanoidH arkanoidItem, uint32_t elemenPos)
{
    return arkanoidItem->elementList[elemenPos].isPresent;
}

ArkanoidStatus arkanoidSetPointsPerFrame(ArkanoidH arkanoidItem, uint8_t pointsPerFrame)
{
    arkanoidItem->pointsPerFrame = pointsPerFrame;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidSetDirection(ArkanoidH arkanoidItem, int32_t dirX, int32_t dirY) {
    arkanoidItem->dirX  = dirX;
    arkanoidItem->dirY  = dirY;
    arkanoidDirectionProcessing(arkanoidItem);
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
