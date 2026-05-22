#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket,
        boost::asio::io_context::strand& log_strand,
        std::vector<std::string>& global_log,
        std::mutex& log_mutex,
        boost::asio::thread_pool& calc_pool);

    void start();

private:
    tcp::socket socket_;
    boost::asio::io_context::strand& log_strand_;
    std::vector<std::string>& global_log_;
    std::mutex& log_mutex_;
    boost::asio::thread_pool& calc_pool_;
    boost::asio::streambuf buffer_;
    boost::asio::steady_timer timer_;

    bool is_closed_ = false;
    void do_read();
    void do_write(const std::string& message);
    void do_calculate(const std::string& input);
    void log_result(const std::string& entry);
    void start_timeout();
    void check_timeout();
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port, int thread_count);
    ~Server();

private:
    tcp::acceptor acceptor_;
    boost::asio::io_context::strand log_strand_;
    std::vector<std::string> global_log_;
    std::mutex log_mutex_;
    boost::asio::thread_pool calc_pool_;

    void do_accept();
};
