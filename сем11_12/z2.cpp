#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <chrono>

namespace asio = boost::asio;
using asio::ip::tcp;
using namespace std;

void test_server(int port) {
    try {
        asio::io_context io;
        tcp::acceptor acc(io, tcp::endpoint(tcp::v4(), port));

        tcp::socket sock(io);
        acc.accept(sock);

        char buf[1024];
        size_t n = sock.read_some(asio::buffer(buf));
        asio::write(sock, asio::buffer(buf, n));
    }
    catch (...) {}
}

string read_from(tcp::socket& sock, const string& name) {
    char buf[1024];
    size_t n = sock.read_some(asio::buffer(buf));
    return name + ": " + string(buf, n);
}

void z2() {
    thread srv1(test_server, 12345);
    thread srv2(test_server, 12346);
    srv1.detach();
    srv2.detach();
    this_thread::sleep_for(chrono::milliseconds(100));

    asio::io_context io;
    tcp::socket sock1(io), sock2(io);
    tcp::resolver res(io);

    asio::connect(sock1, res.resolve("127.0.0.1", "12345"));
    asio::connect(sock2, res.resolve("127.0.0.1", "12346"));
    cout << "Подключено к двум серверам" << endl;

    asio::write(sock1, asio::buffer("Hello 1"));
    asio::write(sock2, asio::buffer("Hello 2"));
    cout << "Сообщения отправлены" << endl;

    string r1 = read_from(sock1, "SOCKET1");
    string r2 = read_from(sock2, "SOCKET2");
    cout << r1 << endl;
    cout << r2 << endl;

    sock1.close();
    sock2.close();
}