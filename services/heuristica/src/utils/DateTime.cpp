#include "DateTime.h"
#include <sstream>

DateTime::DateTime() {}

DateTime::DateTime(int day, int month, int year, int hour, int minutes) {
    this->day = day;
    this->month = month;
    this->year = year;
    this->hour = hour;
    this->minutes = minutes;
    this->seconds = 0;
    initTime();
}

DateTime::DateTime(int day, int month, int year, int hour, int minutes, int seconds) {
    this->day = day;
    this->month = month;
    this->year = year;
    this->hour = hour;
    this->minutes = minutes;
    this->seconds = seconds;
    initTime();
}

void DateTime::initTime() {
    time(&dateTime);
    timeInfo = localtime(&dateTime);
    timeInfo->tm_year = year - 1900;
    timeInfo->tm_mon = month - 1;
    timeInfo->tm_mday = day;
    timeInfo->tm_hour = hour;
    timeInfo->tm_min = minutes;
    timeInfo->tm_sec = seconds;

    dateTime = mktime(timeInfo);
}
        
int DateTime::getDay() {
    return day;
}

int DateTime::getMonth() {
    return month;
}

int DateTime::getYear() {
    return year;
}

int DateTime::getHour() {
    return hour;
}

int DateTime::getMinute() {
    return minutes;
}

int DateTime::getSecond() {
    return seconds;
}

string DateTime::toString() {    
    stringstream ss;
    ss << day << "/" << month << "/" << year << " " << hour << ":" << minutes;
    return ss.str();
}

time_t DateTime::getDateTime() {
    return dateTime;
}