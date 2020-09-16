#ifndef SILVERBULLET_UTIL_UTIL_H_
#define SILVERBULLET_UTIL_UTIL_H_
#include <cstdlib>
#include <string>
ssize_t readn(int fd, void *buff, size_t n);
ssize_t readn(int fd, std::string &inBuffer, bool &zero);
ssize_t readn(int fd, std::string &inBuffer);
ssize_t writen(int fd, void *buff, size_t n);
ssize_t writen(int fd, std::string &sbuff);
void handle_for_sigpipe();
int set_socket_nonblock(int fd);
void set_socket_no_delay(int fd);
void setSocketNoLinger(int fd);
void shut_down_wr(int fd);
int socket_bind_listen(int port);
#endif