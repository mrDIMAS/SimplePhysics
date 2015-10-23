//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>
#include <string.h> 
#include "Vector3.h"
#include "List.h"

typedef struct TSimulationParameters {
    int gjkConvergenceLimit;
    int epaConvergenceLimit;
    float epaTolerance;
    float epaPDTolerance;
} TSimulationParameters;

extern TSimulationParameters gSimulationParameters;

#endif