#ifndef _VERSION_H
#define _VERSION_H

#define GAME_BASE_VERSION "0.0.0"

#ifdef DEBUG
#define GAME_VERSION GAME_BASE_VERSION "-debug"
#else
#define GAME_VERSION GAME_BASE_VERSION
#endif

#endif