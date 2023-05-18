//
// Created by Cheng MingBo on 2023/5/13.
//

#ifndef LIBNET_TIMESTAMP_H
#define LIBNET_TIMESTAMP_H
#include <chrono>
#include <string>
#include "copyable.h"

class Timestamp : public copyable {
  public:
    using Clock = std::chrono::system_clock;
    Timestamp() = default;
    explicit Timestamp(int64_t microSecondsSinceEpochArg)
            : timePoint_(std::chrono::microseconds(microSecondsSinceEpochArg))
    { }
    
    static Timestamp now() {
        return Timestamp(std::chrono::duration_cast<std::chrono::microseconds>(
                                 Clock::now().time_since_epoch()).count()
                        );
    }
    
    static Timestamp invalid() {
        return {};
    }
    
    bool valid() const
    {
        return timePoint_.time_since_epoch().count() > 0;
    }
    
    int64_t microSecondsSinceEpoch() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                timePoint_.time_since_epoch()).count();
    }
    
    time_t secondsSinceEpoch() const
    {
        return std::chrono::duration_cast<std::chrono::seconds>(
                timePoint_.time_since_epoch()).count();
    }
    static const int kMicroSecondsPerSecond = 1000 * 1000;
    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;
  
  private:
    std::chrono::time_point<Clock> timePoint_;
};

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline bool operator!=(Timestamp lhs, Timestamp rhs)
{
    return !(lhs == rhs);
}

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator<=(Timestamp lhs, Timestamp rhs)
{
    return !(rhs < lhs);
}

inline bool operator>(Timestamp lhs, Timestamp rhs)
{
    return rhs < lhs;
}

inline bool operator>=(Timestamp lhs, Timestamp rhs)
{
    return !(lhs < rhs);
}

inline Timestamp addTime(Timestamp timestamp, double seconds) {
    auto delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}
#endif //LIBNET_TIMESTAMP_H
