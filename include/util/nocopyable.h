//
// Created by Cheng MingBo on 2023/5/13.
//

#ifndef LIBNET_NOCOPYABLE_H
#define LIBNET_NOCOPYABLE_H
class noncopyable
{
  public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
  
  protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

#endif //LIBNET_NOCOPYABLE_H
