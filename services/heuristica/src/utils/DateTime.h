#ifndef CLASS_DATETIME_H
#define CLASS_DATETIME_H

#include <iostream>
#include <ctime>
#include <vector>

using namespace std;

class DateTime {
    private:
        int day;
        int month;
        int year;
        int hour;
        int minutes;
        int seconds;
        time_t dateTime;
        struct tm* timeInfo;
        
        void initTime();

    public:
        DateTime();
        DateTime(int day, int month, int year, int hour, int minutes);        
        DateTime(int day, int month, int year, int hour, int minutes, int seconds);        
        int getDay();
        int getMonth();
        int getYear();
        int getHour();
        int getMinute();
        int getSecond();
        string toString();
        time_t getDateTime();
        
        int compare(DateTime *dateTime);
};

#endif