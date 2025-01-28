#ifndef SATELLITES_H
#define SATELLITES_H

#include <stdbool.h>

#include "../fk-engine-core/util.h"

#define NUM_SATELLITES 8

#define SATELLITE_VISIT_DISTANCE 8.0f

typedef struct {
    vec3 position;
    float rotationY;
} Satellite;

void initSatellites();
void quitSatellites();

void createSatellite(vec3 pos, float rotY);
void clearSatellites();

void drawSatellites();

void checkVisitSatellite(vec3* playerPos);
bool checkAllSatellitesVisited();
vec3 getSatellitePosition();

bool hasSatellites();

#endif