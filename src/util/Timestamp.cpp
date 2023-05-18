//
// Created by Cheng MingBo on 2023/5/13.
//


#include "Timestamp.h"
std::string Timestamp::toString() const
{
    char buf[32] = { 0 };
    int64_t microSecondsSinceEpoch = std::chrono::duration_cast<std::chrono::microseconds>(
            timePoint_.time_since_epoch()).count();
    int64_t seconds = microSecondsSinceEpoch / kMicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch % kMicroSecondsPerSecond;
    sprintf(buf, "%ld.%ld", seconds, microseconds);
    std::string str(buf);
    return str;
}

std::string Timestamp::toFormattedString(bool showMicroseconds) const
{
    char buf[64] = { 0 };
    time_t seconds = std::chrono::duration_cast<std::chrono::microseconds>(timePoint_.time_since_epoch()).count() /
                     kMicroSecondsPerSecond;
    tm tm_time;
    gmtime_r(&seconds, &tm_time);
    
    if (showMicroseconds) {
        int64_t microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(
                timePoint_.time_since_epoch()).count() % kMicroSecondsPerSecond;
        snprintf(buf, sizeof(buf), "%04d/%02d/%02d %02d:%02d:%02d.%06ld",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microSeconds);
    }
    else {
        snprintf(buf, sizeof(buf), "%04d/%02d/%02d %02d:%02d:%02d",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    std::string str(buf);
    return str;
}