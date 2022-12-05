#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>
#include <time.h>
#include "TZ.h"

class TimeUtils{
    public:
        TimeUtils();
        
        void setClock(String timezone);
        String getTime();
};
#endif