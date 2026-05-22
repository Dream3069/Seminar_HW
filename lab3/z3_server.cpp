#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket)
        : socket_(std::move(socket)), timer_(socket_.get_executor()) {
        memset(data_, 0, sizeof(data_));
    }

    void start() {
        do_read();
    }

private:
    tcp::socket socket_;
    boost::asio::steady_timer timer_;
    char data_[1024];

    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, 1024),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec && length > 0) {
                    std::string request(data_, length);

                    memset(data_, 0, sizeof(data_));

                    while (!request.empty() && (request.back() == '\n' || request.back() == '\r')) {
                        request.pop_back();
                    }

                    if (!request.empty()) {
                        std::cout << "Получено от клиента: " << request << std::endl;
                        process_request(request);
                    }
                    else {
                        do_read();
                    }
                }
                else if (ec) {
                    std::cerr << "Ошибка чтения: " << ec.message() << std::endl;
                }
            });
    }

    void process_request(const std::string& request) {
        const std::string timer_prefix = "timer ";

        if (request.find(timer_prefix) == 0) {
            std::string time_str = request.substr(timer_prefix.length());
            handle_timer(time_str);
        }
        else {
            do_write("Неизвестная команда. Используйте 'timer N'\n");
        }
    }

    void handle_timer(const std::string& time_str) {
        try {
            int seconds = std::stoi(time_str);
            auto self(shared_from_this());

            std::cout << "Запуск таймера на " << seconds << " секунд." << std::endl;

            std::string ack = "Таймер запущен на " + std::to_string(seconds) + " секунд\n";
            do_write(ack);

            // Запускаем таймер
            timer_.expires_after(std::chrono::seconds(seconds));
            timer_.async_wait([this, self, seconds](const boost::system::error_code& ec) {
                if (!ec) {
                    std::string response = "Прошло " + std::to_string(seconds) + " секунд!\n";
                    do_write(response);
                }
                });

        }
        catch (const std::exception& e) {
            std::string error = "Ошибка: неверный формат числа для таймера. Используйте 'timer 5'\n";
            do_write(error);
        }
    }

    void do_write(const std::string& message) {
        auto self(shared_from_this());
        auto msg_ptr = std::make_shared<std::string>(message);

        boost::asio::async_write(socket_, boost::asio::buffer(*msg_ptr),
            [this, self, msg_ptr](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    do_read();
                }
                else {
                    std::cerr << "Ошибка записи: " << ec.message() << std::endl;
                }
            });
    }
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        std::cout << "Сервер запущен. Ожидание подключений..." << std::endl;
        do_accept();
    }

private:
    tcp::acceptor acceptor_;

    void do_accept() {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "Клиент подключен." << std::endl;
                std::make_shared<Session>(std::move(socket))->start();
            }
            do_accept();
            });
    }
};

void z3_server() {
    try {
        boost::asio::io_context io_context;
        Server server(io_context, 12347);
        io_context.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка сервера: " << e.what() << std::endl;
    }
}