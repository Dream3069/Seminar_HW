#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <chrono>

namespace asio = boost::asio;
using asio::ip::tcp;
using namespace std;

void client() {
    try {
        asio::io_context io;
        tcp::socket sock(io);
        tcp::resolver res(io);

        asio::connect(sock, res.resolve("127.0.0.1", "12345"));
        cout << "клиент подключён" << endl;

        string msg = "Hello, Echo!";
        asio::write(sock, asio::buffer(msg));
        cout << "клиент отправил: " + msg << endl;

        char buf[1024];
        size_t n = sock.read_some(asio::buffer(buf));
        cout << "клиент получил: " + string(buf, n) << endl;

        sock.close();
        cout << "клиент отключён" << endl;

    }
    catch (const exception& e) {
        cout << "ошибка: " + string(e.what()) << endl;
    }
}

void server() {
    asio::io_context io;
    tcp::acceptor acc(io, tcp::endpoint(tcp::v4(), 12345));
    tcp::socket sock(io);

    cout << "сервер запущен на порту 12345" << endl;
    acc.accept(sock);
    cout << "сервер клиент подключился" << endl;

    char buf[1024];
    size_t n = sock.read_some(asio::buffer(buf));
    cout << "сервер получено: " + string(buf, n) << endl;

    asio::write(sock, asio::buffer(buf, n));
    cout << "сервер отправлено эхо" << endl;

    sock.close();
    cout << "сервер завершён" << endl;
}

void z1() {
    thread srv(server);

    this_thread::sleep_for(chrono::milliseconds(100));

    client();

    srv.join();
}