#ifndef PLATFORM_H
#define PLATFORM_H

#include "container.h"
#define PLAT_MAX_NAME 101
#define PLAT_MAX_ENDPOINT 101

typedef struct{
  char name[PLAT_MAX_NAME], endpoint[PLAT_MAX_ENDPOINT];
  Container *main;
} Platform;

Platform* platform_new(char name[PLAT_MAX_NAME], char endpoint[PLAT_MAX_ENDPOINT], Container *main);

void platform_free(Platform *pl);

#endif

