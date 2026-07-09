# 1. 封装 Socket 类

在前面 [RAII 与资源管理](02_RAII_与资源管理.md)章节笔记中的 `FdGuard` 基础上，封装一个完整的 TCP Socket 类，覆盖网络编程所有常用操作：

```cpp
// tcp_socket.hpp
#pragma once
#include "fd_guard.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>

class TcpSocket {
private:
    TcpSocket() : fd_(-1) {}   // 私有默认构造，仅供 from_fd 使用

    FdGuard fd_;

public:
    // 创建一个新的 TCP socket
    TcpSocket() : fd_(::socket(AF_INET, SOCK_STREAM, 0)) {
        if (!fd_) {
            throw std::runtime_error(
                std::string("socket() failed: ") + strerror(errno));
        }
    }

    // 从已有 fd 接管（用于 accept 返回的 fd）
    static TcpSocket from_fd(int fd) {
        TcpSocket s;
        s.fd_.reset(fd);   // 先关闭构造函数创建的 fd，再接管新 fd
        return s;
    }

    // 移动语义：默认即可（FdGuard 已实现）
    TcpSocket(TcpSocket&&)            = default;
    TcpSocket& operator=(TcpSocket&&) = default;

    // ---- Socket 选项 ----

    // 服务器必加：允许重启后立即 bind 相同端口
    TcpSocket& reuse_addr(bool on = true) {
        int opt = on ? 1 : 0;
        setsockopt(fd_.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        return *this;   // 返回 *this，支持链式调用
    }

    // 禁用 Nagle 算法，降低延迟
    TcpSocket& no_delay(bool on = true) {
        int opt = on ? 1 : 0;
        setsockopt(fd_.get(), IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
        return *this;
    }

    // 设置接收/发送缓冲区大小
    TcpSocket& recv_buf(int bytes) {
        setsockopt(fd_.get(), SOL_SOCKET, SO_RCVBUF, &bytes, sizeof(bytes));
        return *this;
    }
    TcpSocket& send_buf(int bytes) {
        setsockopt(fd_.get(), SOL_SOCKET, SO_SNDBUF, &bytes, sizeof(bytes));
        return *this;
    }

    // ---- 服务器端操作 ----

    void bind(uint16_t port, const char* ip = nullptr) {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        if (ip) {
            if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1)
                throw std::invalid_argument("bad IP address");
        } else {
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
        }

        if (::bind(fd_.get(), (sockaddr*)&addr, sizeof(addr)) < 0) {
            throw std::runtime_error(
                std::string("bind() failed: ") + strerror(errno));
        }
    }

    void listen(int backlog = 128) {
        if (::listen(fd_.get(), backlog) < 0) {
            throw std::runtime_error(
                std::string("listen() failed: ") + strerror(errno));
        }
    }

    // accept：返回新的 TcpSocket 对象（RAII，无需手动 close）
    TcpSocket accept(std::string* peer_addr = nullptr) {
        sockaddr_in client{};
        socklen_t len = sizeof(client);

        int conn_fd = ::accept(fd_.get(), (sockaddr*)&client, &len);
        if (conn_fd < 0) {
            throw std::runtime_error(
                std::string("accept() failed: ") + strerror(errno));
        }

        if (peer_addr) {
            char buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client.sin_addr, buf, sizeof(buf));
            *peer_addr = std::string(buf) + ":"
                       + std::to_string(ntohs(client.sin_port));
        }

        return TcpSocket::from_fd(conn_fd);
    }

    // ---- 客户端操作 ----

    void connect(const char* ip, uint16_t port) {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1)
            throw std::invalid_argument("bad IP address");

        if (::connect(fd_.get(), (sockaddr*)&addr, sizeof(addr)) < 0) {
            throw std::runtime_error(
                std::string("connect() failed: ") + strerror(errno));
        }
    }

    // ---- 数据收发 ----

    // 发送全部数据（处理 send 可能只发部分的情况）
    void send_all(const void* buf, size_t len) {
        const char* ptr = static_cast<const char*>(buf);
        while (len > 0) {
            ssize_t n = ::send(fd_.get(), ptr, len, MSG_NOSIGNAL);
            if (n < 0) {
                if (errno == EINTR) continue;
                throw std::runtime_error(
                    std::string("send() failed: ") + strerror(errno));
            }
            ptr += n;
            len -= n;
        }
    }

    // 接收恰好 len 字节（返回实际收到字节数，0 表示对端关闭）
    ssize_t recv_some(void* buf, size_t len) {
        while (true) {
            ssize_t n = ::recv(fd_.get(), buf, len, 0);
            if (n < 0) {
                if (errno == EINTR) continue;
                throw std::runtime_error(
                    std::string("recv() failed: ") + strerror(errno));
            }
            return n;   // 0 = EOF, >0 = 收到字节数
        }
    }

    int get() const noexcept { return fd_.get(); }
};
```

## 1.1 网络资源的管理

不是所有资源都能直接用 RAII 类包装，智能指针提供了通用方案：

```cpp
#include <memory>
#include <functional>

// ---- unique_ptr：独占所有权，零开销 ----

// 自定义删除器：管理 malloc 分配的 C 风格缓冲区
struct FreeDeleter {
    void operator()(void* p) const noexcept { std::free(p); }
};
using MallocBuf = std::unique_ptr<char, FreeDeleter>;

MallocBuf make_buffer(size_t size) {
    void* p = std::malloc(size);
    if (!p) throw std::bad_alloc();
    return MallocBuf(static_cast<char*>(p));
}

// 用 unique_ptr 管理 fd（另一种轻量写法）
struct CloseFd {
    void operator()(int* fd) const noexcept {
        if (fd && *fd >= 0) ::close(*fd);
        delete fd;
    }
};
// 注意：unique_ptr 管理指针，不直接管理整数
// 所以 FdGuard 类才是 fd 的最佳管理方案（unique_ptr 有一层间接）

// ---- shared_ptr：共享所有权，有引用计数开销 ----

// 网络连接通常需要 shared_ptr：
// 一个连接对象可能同时被"读线程"和"写线程"持有
class Connection {
public:
    using Ptr = std::shared_ptr<Connection>;

    static Ptr create(int fd, const std::string& peer) {
        // 注意：用 make_shared 而非 new，一次内存分配
        return std::make_shared<Connection>(fd, peer);
    }

    Connection(int fd, std::string peer)
        : sock_(fd), peer_(std::move(peer)) {}

    void send(const std::string& msg) {
        sock_.send_all(msg.data(), msg.size());
    }

    const std::string& peer() const { return peer_; }

private:
    TcpSocket   sock_;
    std::string peer_;
};

// ---- shared_ptr + 自定义删除器：管理任意资源 ----

// 管理 SSL_CTX（OpenSSL 上下文，第17课会用到）
// 演示自定义删除器的通用模式
struct FakeSSLCtx { int dummy; };
void FakeSSLCtx_free(FakeSSLCtx* ctx) { delete ctx; }

void ssl_demo() {
    auto ssl_ctx = std::shared_ptr<FakeSSLCtx>(
        new FakeSSLCtx{},
        FakeSSLCtx_free    // 自定义删除器
    );
    // ssl_ctx 引用计数归零时，自动调用 FakeSSLCtx_free
}

// ---- weak_ptr：打破循环引用 ----

// 连接管理器持有连接，连接反向引用管理器
// 如果都用 shared_ptr 会循环引用，内存泄漏
class ConnectionManager;

class ManagedConnection {
public:
    // 用 weak_ptr 持有 manager，不增加引用计数
    std::weak_ptr<ConnectionManager> manager_;
    TcpSocket sock_;

    explicit ManagedConnection(int fd) : sock_(fd) {}

    void notify_manager() {
        // lock() 尝试将 weak_ptr 提升为 shared_ptr
        if (auto mgr = manager_.lock()) {
            // mgr 有效，manager 还活着
            // mgr->on_data(*this);
        }
        // 如果 manager 已销毁，lock() 返回空，安全地跳过
    }
};
```

---

# 2. 一个完整的 Echo 服务器

把本节所有概念整合成一个可运行的单文件 echo 服务器：

```cpp
// raii_echo_server.cpp
// 编译：g++ -std=c++17 -Wall -Wextra raii_echo_server.cpp -o echo_server

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cerrno>

// ===== FdGuard（第一课封装的复用）=====
class FdGuard {
public:
    explicit FdGuard(int fd = -1) noexcept : fd_(fd) {}
    ~FdGuard() noexcept { if (fd_ >= 0) ::close(fd_); }
    FdGuard(const FdGuard&)            = delete;
    FdGuard& operator=(const FdGuard&) = delete;
    FdGuard(FdGuard&& o) noexcept : fd_(std::exchange(o.fd_, -1)) {}
    FdGuard& operator=(FdGuard&& o) noexcept {
        if (this != &o) { if (fd_ >= 0) ::close(fd_); fd_ = std::exchange(o.fd_, -1); }
        return *this;
    }
    int  get()   const noexcept { return fd_; }
    bool valid() const noexcept { return fd_ >= 0; }
    explicit operator bool() const noexcept { return valid(); }
    void reset(int fd = -1) noexcept { if (fd_ >= 0) ::close(fd_); fd_ = fd; }
private:
    int fd_;
};

// ===== 信号安全的 SIGCHLD 处理 =====
static void reap_children(int) {
    int saved = errno;
    while (waitpid(-1, nullptr, WNOHANG) > 0);
    errno = saved;
}

// ===== 每个连接的处理逻辑（子进程中运行）=====
void handle_connection(FdGuard conn, std::string peer) {
    // conn 在函数退出时自动 close，无论正常还是异常
    std::cout << "[" << peer << "] connected\n";

    auto buf = std::make_unique<char[]>(4096);   // RAII：退出时自动释放

    while (true) {
        ssize_t n = recv(conn.get(), buf.get(), 4096, 0);
        if (n == 0) {
            std::cout << "[" << peer << "] disconnected\n";
            break;
        }
        if (n < 0) {
            if (errno == EINTR) continue;
            std::cerr << "[" << peer << "] recv error: " << strerror(errno) << "\n";
            break;
        }

        // echo 回去
        const char* ptr = buf.get();
        size_t remaining = n;
        while (remaining > 0) {
            ssize_t sent = send(conn.get(), ptr, remaining, MSG_NOSIGNAL);
            if (sent <= 0) goto done;
            ptr       += sent;
            remaining -= sent;
        }
    }
done:;
    // conn 析构 → close(fd) 自动发生，四次挥手由内核完成
}

// ===== 主服务器 =====
int main(int argc, char* argv[]) {
    uint16_t port = (argc > 1) ? static_cast<uint16_t>(atoi(argv[1])) : 8080;

    // 忽略 SIGPIPE：客户端断开时不要杀死进程
    signal(SIGPIPE, SIG_IGN);

    // 注册 SIGCHLD：自动回收子进程
    struct sigaction sa{};
    sa.sa_handler = reap_children;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, nullptr);

    // 创建监听 socket（RAII：退出时自动 close）
    FdGuard listen_fd(::socket(AF_INET, SOCK_STREAM, 0));
    if (!listen_fd) { perror("socket"); return 1; }

    // 链式配置 socket 选项
    auto set_opt = [&](int level, int opt, int val) {
        setsockopt(listen_fd.get(), level, opt, &val, sizeof(val));
    };
    set_opt(SOL_SOCKET,  SO_REUSEADDR, 1);   // 重启后可立即 bind
    set_opt(IPPROTO_TCP, TCP_NODELAY,  1);   // 禁 Nagle

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_fd.get(), (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    if (listen(listen_fd.get(), 128) < 0) {
        perror("listen"); return 1;
    }

    std::cout << "Echo server listening on port " << port << "\n";

    while (true) {
        sockaddr_in client{};
        socklen_t clen = sizeof(client);

        // accept 返回新 fd，立即用 FdGuard 包装
        FdGuard conn(::accept(listen_fd.get(), (sockaddr*)&client, &clen));
        if (!conn) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        // 获取对端地址（用于日志）
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
        std::string peer = std::string(ip) + ":" + std::to_string(ntohs(client.sin_port));

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;   // conn 析构，自动关闭连接
        }

        if (pid == 0) {
            // ===== 子进程 =====
            listen_fd.reset();   // 子进程不需要监听 fd，立即关闭
            // conn 的所有权逻辑上归子进程，移动进函数
            handle_connection(std::move(conn), peer);
            // handle_connection 返回时 conn 已析构（fd 已 close）
            _exit(0);
        }

        // ===== 父进程 =====
        // conn 在父进程这里析构（fd 引用计数 -1）
        // 子进程那边还持有一个引用
        // 父进程的 conn 在这里作用域结束，自动 close
    }
}
```

测试方法：

```bash
# 编译并运行
g++ -std=c++17 -Wall raii_echo_server.cpp -o echo_server
./echo_server 8080

# 另一个终端测试
echo "hello RAII" | nc 127.0.0.1 8080

# 验证没有 fd 泄漏
ls -la /proc/$(pgrep echo_server)/fd | wc -l
```

