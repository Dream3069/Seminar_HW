// Sem9_10.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <memory>

using namespace std;
using namespace boost::asio;
using ip::tcp;

atomic<int> global_message_count(0);
mutex cout_mutex;

void safe_print(const string& msg) {
    lock_guard<mutex> lock(cout_mutex);
    cout << msg << endl;
}

class Session : public enable_shared_from_this<Session> {
    tcp::socket socket_;
    char data_[1024];
    shared_ptr<strand<tcp::socket::executor_type>> strand_;

public:
    Session(tcp::socket sock, shared_ptr<strand<tcp::socket::executor_type>> str)
        : socket_(move(sock)), strand_(str) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self = shared_from_this();
        socket_.async_read_some(buffer(data_, 1024),
            bind_executor(*strand_,
                [this, self](boost::system::error_code ec, size_t len) {
                    if (!ec && len > 0) {
                        string msg(data_, len);
                        safe_print("сервер: получено: " + msg);

                        global_message_count++;
                        safe_print("сервер: всего сообщений: " + to_string(global_message_count));

                        do_write(len);
                    }
                    else if (ec == boost::asio::error::eof) {
                        safe_print("сервер: клиент отключился");
                    }
                    else if (ec) {
                        safe_print("сервер: ошибка: " + ec.message());
                    }
                }));
    }

    void do_write(size_t len) {
        auto self = shared_from_this();
        async_write(socket_, buffer(data_, len),
            bind_executor(*strand_,
                [this, self](boost::system::error_code ec, size_t) {
                    if (!ec) {
                        safe_print("сервер: отправлено эхо");
                        do_read();
                    }
                }));
    }
};

class Server {
    tcp::acceptor acceptor_;
    shared_ptr<strand<tcp::socket::executor_type>> strand_;

public:
    Server(io_context& io, short port)
        : acceptor_(io, tcp::endpoint(tcp::v4(), port)) {
        strand_ = make_shared<strand<tcp::socket::executor_type>>(io.get_executor());
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    safe_print("сервер: новый клиент подключился");
                    make_shared<Session>(move(socket), strand_)->start();
                }
                do_accept();
            });
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    try {
        io_context io;
        Server server(io, 12345);
        safe_print("сервер: запущен на порту 12345");

        vector<thread> threads;
        for (int i = 0; i < 4; i++) {
            threads.emplace_back([&io]() { io.run(); });
        }
        for (auto& t : threads) t.join();

    }
    catch (exception& e) {
        cerr << "ошибка: " << e.what() << endl;
    }
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
