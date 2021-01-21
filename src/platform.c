#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"

Platform* platform_new(char name[PLAT_MAX_NAME], char endpoint[PLAT_MAX_ENDPOINT], Container *main){
  Platform *p = (Platform*) malloc(sizeof(Platform));

  strcpy(p->name, name);
  strcpy(p->endpoint, endpoint);
  p->main = main;

  return p;
}

void platform_free(Platform *pl){
  free(pl);
  /*
   * It could be possible to free internal components,
   * like Container *main, agent of *main, etc.
   * Implementation should be done later...
   */
}