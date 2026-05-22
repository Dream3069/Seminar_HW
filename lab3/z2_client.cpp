#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void z2_client() {
    try {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "12346");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        std::cout << "Подключено к серверу." << std::endl;
        std::cout << "Сервер поддерживает асинхронное вычисление факториала." << std::endl;

        while (true) {
            std::cout << "\nВведите число для вычисления факториала (0 для выхода): ";
            std::string input;
            std::cin >> input;

            if (input == "0" || input == "exit" || input == "quit") {
                break;
            }

            input += "\n";
            boost::asio::write(socket, boost::asio::buffer(input));

            char ack[256] = { 0 };
            size_t ack_length = socket.read_some(boost::asio::buffer(ack, 256));
            std::cout << "Сервер: ";
            std::cout.write(ack, ack_length);
            std::cout << std::endl;

            char response[256] = { 0 };
            size_t response_length = socket.read_some(boost::asio::buffer(response, 256));
            std::cout << "Результат: ";
            std::cout.write(response, response_length);
            std::cout << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка клиента: " << e.what() << std::endl;
    }
}