#ifndef SILVERBULLET_util_HTTPDATA_H_
#define SILVERBULLET_util_HTTPDATA_H_
#include "timer.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>

namespace SB
{
    namespace util
    {
        class EventLoop;
        class TimerNode;
        class Channel;

        enum ProcessState
        {
            STATE_PARSE_URI = 1,
            STATE_PARSE_HEADERS,
            STATE_RECV_BODY,
            STATE_ANALYSIS,
            STATE_FINISH
        };

        enum URIState
        {
            PARSE_URI_AGAIN = 1,
            PARSE_URI_ERROR,
            PARSE_URI_SUCCESS,
        };

        enum HeaderState
        {
            PARSE_HEADER_SUCCESS = 1,
            PARSE_HEADER_AGAIN,
            PARSE_HEADER_ERROR
        };

        enum AnalysisState
        {
            ANALYSIS_SUCCESS = 1,
            ANALYSIS_ERROR
        };

        enum ParseState
        {
            H_START = 0,
            H_KEY,
            H_COLON,
            H_SPACES_AFTER_COLON,
            H_VALUE,
            H_CR,
            H_LF,
            H_END_CR,
            H_END_LF
        };

        enum ConnectionState
        {
            H_CONNECTED = 0,
            H_DISCONNECTING,
            H_DISCONNECTED
        };

        enum HttpMethod
        {
            METHOD_POST = 1,
            METHOD_GET,
            METHOD_HEAD
        };

        enum HttpVersion
        {
            HTTP_10 = 1,
            HTTP_11
        };
        /**
     * @brief 消息类型
     * 
     */
        class MimeType
        {
        private:
            static void init();
            static std::unordered_map<std::string, std::string> mime;
            MimeType();
            MimeType(const MimeType &m);

        public:
            static std::string get_mime(const std::string &suffix);

        private:
            static pthread_once_t once_control;
        };

        class HttpMessage : public std::enable_shared_from_this<HttpMessage>
        {
        public:
            HttpMessage(EventLoop *loop, int connfd);
            ~HttpMessage() { close(fd_); }
            void reset();
            void seperateTimer();
            void linkTimer(std::shared_ptr<TimerNode> mtimer)
            {
                // shared_ptr重载了bool, 但weak_ptr没有
                timer_ = mtimer;
            }
            std::shared_ptr<Channel> channel() { return channel_; }
            EventLoop *getLoop() { return loop_; }
            void handle_close();
            void add_event();

        private:
            EventLoop *loop_;
            std::shared_ptr<Channel> channel_;
            int fd_;
            std::string in_buff_;
            std::string out_buff_;
            bool error_;
            ConnectionState connection_state_;

            HttpMethod method_;
            HttpVersion version_;
            std::string file_name_;
            std::string path_;
            int read_pos_;
            ProcessState state_;
            ParseState hState_;
            bool keep_alive_;
            std::map<std::string, std::string> headers_;
            std::weak_ptr<TimerNode> timer_;

            void handle_read();
            void handle_write();
            void handle_env();
            void handle_err_(int fd, int err_num, std::string short_msg);
            URIState parse_uri();
            HeaderState parse_headers();
            AnalysisState analysis_request();
        };
    } // namespace util
} // namespace SB

#endif