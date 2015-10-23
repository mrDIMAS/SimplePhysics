//////////////////////////////////////////
//         GJK-EPA Algorithm impl.      //
//          Simple Physics impl.        //
//            (C) mrDIMAS 2015          //
//////////////////////////////////////////

#include "Common.h"

TSimulationParameters gSimulationParameters = {     
    .gjkConvergenceLimit = 15,
    .epaConvergenceLimit = 15,
    .epaTolerance = 0.001f,
    .epaPDTolerance = 0.0001f 
};