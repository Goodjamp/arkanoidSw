#include "stdint.h"
#include "stdbool.h"

#include "arkonoidEngeen.h"

typedef struct ArkanoidDeskriptor {
    uint8_t        speed;
    uint16_t       platformPos;
    PlatformSide   plarformSide;
    uint16_t       plarformSize;
    ElementDescr  *elementList;
    uint16_t       elementQuantity;
    ArkanoidStatus status;
    uint16_t       x;
    uint16_t       y;
} ArkanoidDeskriptor;

static uint8_t itemCount;
static ArkanoidDeskriptor arkanoidExample[ARKANOID_ITEMS_QUANTITY];

ArkanoidH arkanoidInit(ElementDescr *elementList, uint16_t elementQuantity, PlatformSide plarformSide, uint16_t platformSize, uint8_t speed)
{
    if((itemCount + 1) >= ARKANOID_ITEMS_QUANTITY) {
        return NULL;
    }
    arkanoidExample[itemCount++].speed = speed;
    arkanoidExample[itemCount].platformPos = (plarformSide == PLATFORM_UP || plarformSide == PLATFORM_DOWN ) ? (AREA_WIDTH / 2) : (AREA_HEIGHT / 2);
    arkanoidExample[itemCount].plarformSide = plarformSide;
    arkanoidExample[itemCount].elementList = elementList;
    arkanoidExample[itemCount].elementQuantity = elementQuantity;
    return &arkanoidExample[itemCount];
}

ArkanoidStatus arkanoidSetSpeed(ArkanoidH arkanoidItem, uint8_t speed)
{
    arkanoidItem->speed = speed;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidUpdate(ArkanoidH arkanoidItem, uint32_t time)
{
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidSetPlatformSize(ArkanoidH arkanoidItem, uint16_t size)
{
    arkanoidItem->plarformSize = size;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidSetPlatformPos(ArkanoidH arkanoidItem, uint16_t pos)
{
    arkanoidItem->platformPos = pos;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidGetBallPos(ArkanoidH arkanoidItem, uint16_t *x, uint16_t *y)
{
    *x = arkanoidItem->x;
    *y = arkanoidItem->y;
    return arkanoidItem->status;
}

ArkanoidStatus arkanoidGetStatus(ArkanoidH arkanoidItem)
{
    return arkanoidItem->status;
}

#include "stdio.h"

typedef struct Line {
    int32_t x1;
    int32_t x2;
    int32_t y1;
    int32_t y2;
}Line;

typedef struct Vector {
    int32_t x;
    int32_t y;
}Vector;

bool isCrose(Line *line1, Line *line2);

void testCrose(void)
{
    Line line1 = {
        .x1 = 0,
        .y1 = 0,
        .x2 = 5,
        .y2 = 5
    };
    Line line2 = {
        .x1 = 4,
        .y1 = 30,
        .x2 = 2,
        .y2 = 2
    };
    printf("result =%s \n", (isCrose(&line1, &line2)) ? ("Crose") : ("No Cros"));
}

bool isCrose(Line *line1, Line *line2)
{
    // line 2 cross line 1
    Vector v1 = {
        .x = line1->x2 - line1->x1,
        .y = line1->y2 - line1->y1,
    };
    Vector v11 = {
        .x = line2->x1 - line1->x1,
        .y = line2->y1 - line1->y1,
    };
    Vector v12 = {
        .x = line2->x2 - line1->x1,
        .y = line2->y2 - line1->y1,
    };
    int32_t z11 = v1.x * v11.y - v1.y * v11.x;
    int32_t z12 = v1.x * v12.y - v1.y * v12.x;
    printf("z11  = %d\n", z11);
    printf("z12  = %d\n", z12);
    if((z11 * z12) > 0) {
        return false;
    }
    // line 1 cross line 2
    Vector v2 = {
        .x = line2->x2 - line2->x1,
        .y = line2->y2 - line2->y1,
    };
    Vector v21 = {
        .x = line1->x1 - line2->x1,
        .y = line1->y1 - line2->y1,
    };
    Vector v22 = {
        .x = line1->x2 - line2->x1,
        .y = line1->y2 - line2->y1,
    };
    int32_t z21 = v2.x * v21.y - v2.y * v21.x;
    int32_t z22 = v2.x * v22.y - v2.y * v22.x;
    printf("z21  = %d\n", z21);
    printf("z22  = %d\n", z22);
    if((z21 * z22) > 0) {
        return false;
    }



    return true;
}

