#include "redis.h"

redis_clt *redis_clt::m_redis_instance = new redis_clt();

redis_clt *redis_clt::getinstance()
{
    return m_redis_instance;
}

string redis_clt::getReply(string m_command)
{

    m_redisReply = (redisReply *)redisCommand(m_redisContext, m_command.c_str());

    if (m_redisReply->len > 0)
    {
        string temp = string(m_redisReply->str);
        freeReplyObject(m_redisReply);
        return temp;
    }
    else
    {
        freeReplyObject(m_redisReply);
        return "";
    }
}

redis_clt::redis_clt()
{
    timeout = {2, 0};
    m_redisContext = (redisContext *)redisConnectWithTimeout("127.0.0.1", 6379, timeout);
    m_redisReply = nullptr;
}
/*
int main()
{
    redis_clt *m_r = redis_clt::getinstance();
    string temp = m_r->getUserpasswd("32");
    cout << temp << endl;
    return 0;
}*/