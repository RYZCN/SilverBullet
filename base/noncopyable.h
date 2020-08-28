#ifndef SILVERBULLET_NONCOPYABLE_H_
#define SILVERBULLET_NONCOPYABLE_H_
class noncopyable
{
public:
    noncopyable(const noncopyable &l) = delete;
    void operator=(const noncopyable &rhs) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
#endif