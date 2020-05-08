#ifndef _ARKONOID_H_
#define _ARKONOID_H_

#include "stdint.h"
#include "stdbool.h"

#define ARKANOID_ITEMS_QUANTITY 2
//#define AREA_HEIGHT             128
//#define AREA_WIDTH              64
#define BALL_R                  3
#define ELEMENT_LIST_MAX_SIZE   40

typedef enum{
    PLATFORM_UP,
    PLATFORM_DOWN,
    PLATFORM_LEFT,
    PLATFORM_RIGHT,
}PlatformSide;

typedef enum {
    ELEMENT_SIMPLE,
    ELEMENT_STRONG,
} ElementType;

typedef enum {
    STATUS_GAME,
    STATUS_CRASH,
    STATUS_ERRO,
} ArkanoidStatus;

typedef struct {
    int32_t    x1;
    int32_t    y1;
    int32_t    x2;
    int32_t    y2;
    ElementType type;
    bool        isPresent;
} ElementDescr;

typedef struct ArkanoidConfig{
    ElementDescr *elementList;
    int32_t      platformHeight;
    int32_t      platformWidth;
    int32_t      areaHeight;
    int32_t      areaWidth;
    int32_t      platformPosX;
    int32_t      platformPosY;
    int32_t      ballSpeed;
    int32_t      dirX;
    int32_t      dirY;
    uint16_t     elementQuantity;
    uint32_t     pointsPerFrame;
}ArkanoidConfig;

typedef struct ArkanoidDeskriptor *ArkanoidH;

ArkanoidH      arkanoidInit(ArkanoidConfig arkanoidConfig);
ArkanoidStatus arkanoidSetSpeed(ArkanoidH arkanoidItem, uint8_t ballSpeed);
ArkanoidStatus arkanoidUpdate(ArkanoidH arkanoidItem, uint32_t time);
ArkanoidStatus arkanoidSetDirection(ArkanoidH arkanoidItem, int32_t dirLen, int32_t dirX, int32_t dirY);
ArkanoidStatus arkanoidSetPlatformSize(ArkanoidH arkanoidItem, int32_t width, int32_t height);
ArkanoidStatus arkanoidSetPlatformPos(ArkanoidH arkanoidItem, int32_t x, int32_t y);
ArkanoidStatus arkanoidSetBallPos(ArkanoidH arkanoidItem, int32_t x, int32_t y);
ArkanoidStatus arkanoidGetBallPos(ArkanoidH arkanoidItem, int32_t *x, int32_t *y);
ArkanoidStatus arkanoidGetStatus(ArkanoidH arkanoidItem);

/*test*/
void testCrose(void);
#endif
