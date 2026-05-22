#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void z3_client() {
    try {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "12347");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        std::cout << "Подключено к серверу." << std::endl;
        std::cout << "Сервер поддерживает команду 'timer N' (где N - количество секунд)." << std::endl;

        while (true) {
            std::cout << "\nВведите 'timer N' (N - натуральное число секунд) или 'exit' для выхода: ";
            std::string message;
            std::getline(std::cin, message);

            // Проверяем на пустую строку
            if (message.empty()) {
                continue;
            }

            if (message == "exit" || message == "quit" || message == "выход") {
                break;
            }

            std::string send_msg = message + "\n";
            boost::asio::write(socket, boost::asio::buffer(send_msg));

            for (int i = 0; i < 2; i++) {
                char response[1024] = { 0 };
                size_t length = socket.read_some(boost::asio::buffer(response, 1024));

                if (length > 0) {
                    std::cout << "Ответ от сервера: ";
                    std::cout.write(response, length);
                    std::cout << std::endl;
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка клиента: " << e.what() << std::endl;
    }
}