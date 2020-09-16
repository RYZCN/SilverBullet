#ifndef SILVERBULLET_BASE_NONCOPYABLE_H_
#define SILVERBULLET_BASE_NONCOPYABLE_H_
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