#ifndef _ARKONOID_H_
#define _ARKONOID_H_

#include "stdint.h"
#include "stdbool.h"

#define ARKANOID_ITEMS_QUANTITY 2
#define AREA_HEIGHT             128
#define AREA_WIDTH              64
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
    uint16_t    x;
    uint16_t    y;
    ElementType type;
    bool        isPresent;
} ElementDescr;

typedef struct ArkanoidDeskriptor *ArkanoidH;

ArkanoidH      arkanoidInit(ElementDescr *elementList, uint16_t elementQuantity, PlatformSide plarformSide, uint16_t platformSize, uint8_t speed);
ArkanoidStatus arkanoidSetSpeed(ArkanoidH arkanoidItem, uint8_t speed);
ArkanoidStatus arkanoidUpdate(ArkanoidH arkanoidItem, uint32_t time);
ArkanoidStatus arkanoidSetPlatformSize(ArkanoidH arkanoidItem, uint16_t size);
ArkanoidStatus arkanoidSetPlatformPos(ArkanoidH arkanoidItem, uint16_t pos);
ArkanoidStatus arkanoidGetBallPos(ArkanoidH arkanoidItem, uint16_t *x, uint16_t *y);
ArkanoidStatus arkanoidGetStatus(ArkanoidH arkanoidItem);

/*test*/
void testCrose(void);
#endif
