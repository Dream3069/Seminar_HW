#include "z4_server.hpp"
#include <chrono>

Session::Session(tcp::socket socket,
    boost::asio::io_context::strand& log_strand,
    std::vector<std::string>& global_log,
    std::mutex& log_mutex,
    boost::asio::thread_pool& calc_pool)
    : socket_(std::move(socket)),
    log_strand_(log_strand),
    global_log_(global_log),
    log_mutex_(log_mutex),
    calc_pool_(calc_pool),
    timer_(socket_.get_executor()) {}

void Session::start() {
    start_timeout();
    do_read();
}

void Session::start_timeout() {
    timer_.expires_after(std::chrono::seconds(30));
    check_timeout();
}

void Session::check_timeout() {
    auto self = shared_from_this();
    timer_.async_wait([this, self](const boost::system::error_code& ec) {
        if (!ec && !is_closed_) {
            std::cout << "Клиент не ответил в течение 30 секунд. Закрываем соединение." << std::endl;
            boost::system::error_code ignored_ec;
            socket_.shutdown(tcp::socket::shutdown_both, ignored_ec);
            socket_.close(ignored_ec);
            is_closed_ = true;
        }
        });
}

void Session::log_result(const std::string& entry) {
    auto self = shared_from_this();
    boost::asio::post(log_strand_, [this, self, entry]() {
        std::lock_guard<std::mutex> lock(log_mutex_);
        global_log_.push_back(entry);
        std::cout << "[LOG]: " << entry
            << " [Записей в журнале: " << global_log_.size() << "]" << std::endl;
        });
}

void Session::do_read() {
    auto self = shared_from_this();
    boost::asio::async_read_until(socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                timer_.cancel();
                std::istream is(&buffer_);
                std::string line;
                std::getline(is, line);
                do_calculate(line);
            }
            else if (ec != boost::asio::error::operation_aborted) {
                is_closed_ = true;
            }
        });
}

void Session::do_calculate(const std::string& input) {
    auto self = shared_from_this();
    boost::asio::post(calc_pool_, [this, self, input]() {
        try {
            int n = std::stoi(input);

            if (n < 0) {
                throw std::out_of_range("Факториал отрицательного числа не определен");
            }

            unsigned long long result = 1;
            for (int i = 1; i <= n; ++i) {
                if (result > ULLONG_MAX / i) {
                    throw std::overflow_error("Результат превышает максимальное значение");
                }
                result *= i;
            }

            std::string result_str = "Факториал " + input + " равен " + std::to_string(result);
            log_result(result_str);
            do_write(result_str + "\n");
        }
        catch (const std::exception& e) {
            log_result("Ошибка обработки ввода '" + input + "': " + e.what());
            do_write("Ошибка: " + std::string(e.what()) + "\n");
        }
        });
}

void Session::do_write(const std::string& message) {
    auto self = shared_from_this();
    boost::asio::async_write(socket_, boost::asio::buffer(message),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                start_timeout();
                do_read();
            }
            else {
                is_closed_ = true;
            }
        });
}

Server::Server(boost::asio::io_context& io_context, short port, int thread_count)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
    log_strand_(io_context),
    calc_pool_(thread_count) {
    do_accept();
}

Server::~Server() {
    boost::system::error_code ec;
    acceptor_.close(ec);
    calc_pool_.join();
}

void Server::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::make_shared<Session>(std::move(socket), log_strand_, global_log_, log_mutex_, calc_pool_)->start();
        }
        do_accept();
        });
}