#include "socket.hpp"

Session::Session(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
    : sockfd_(sockfd), addr_(addr), addrlen_(addrlen) {}

Session::~Session() {}

int Session::getSockFd() const {
    return sockfd_;
}

void Session::addSendQueue(const std::string& buffer) {
    send_queue_.push_back(buffer);
}

TcpSession::TcpSession(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
    : Session(sockfd, addr, addrlen) {}

bool TcpSession::send() {
    if (send_queue_.empty()) {
        return true;
    }

    std::string buffer     = send_queue_.front();
    ssize_t     bytes_sent = ::send(sockfd_, buffer.c_str(), buffer.length(), MSG_DONTWAIT);

    if (bytes_sent == -1) {
        Logger::instance().log("ERROR: Failed to send to socket");
        return false;
    }

    send_queue_.pop_front();

    if (static_cast<size_t>(bytes_sent) != buffer.length()) {
        send_queue_.push_front(buffer.substr(bytes_sent));
    }

    if (send_queue_.empty()) {
        Logger::instance().log("Finished sending");
        return true;
    }

    return false;
}

std::pair<std::string, ssize_t> TcpSession::recv(int client) const {
    char buffer[READ_BUFFER_SIZE];
    ssize_t bytes_received;
    ssize_t total_bytes_received = 0;
    std::ifstream ifs;
    std::vector<char> buffer_data = std::vector<char>(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    try {
        do {
            bytes_received = ::recv(client, buffer, READ_BUFFER_SIZE, 0);
            if (bytes_received > 0) {
                //buffer_data.insert(buffer_data.end(), buffer, buffer + bytes_received);
                for (ssize_t i = 0; i < bytes_received; ++i) {
                    buffer_data.push_back(buffer[i]);
                }
                //buffer_data.insert(buffer_data.end(), buffer, buffer + bytes_received);
                total_bytes_received += bytes_received;
            }
        } while (bytes_received == READ_BUFFER_SIZE);
    } catch (std::exception &e) {
        std::cerr << "DAT STRING ERROR: " << e.what() << std::endl;
    }

    if (bytes_received == -1) {
        Logger::instance().log("Error: Failed to receive from socket");
    }

    //std::cout << "---TEST OF VECTOR OF CHAR---" << std::endl;
    //for (std::vector<char>::iterator it = buffer_data.begin(); it != buffer_data.end(); ++it) { 
    //    std::cout << *it;
    //}
    //std::cout << "---END OF TEST VECTOR OF CHAR---" << std::endl;
    //ifs.clear();
    //ifs.seekg(0,std::ios::beg);
    //std::string buffer_str(buffer_data.begin(), buffer_data.end());
    //std::string buffer_str = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    //std::istringstream iss(std::string(buffer_data.begin(), buffer_data.end()));
    //std::string buffer_str;
    //std::string buffer_str = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    //buffer_str.reserve(buffer_data.size());
    //std::copy(buffer_data.begin(), buffer_data.end(), std::back_inserter(buffer_str));
    //buffer_str.assign(std::istreambuf_iterator<char>(iss), std::istreambuf_iterator<char>());
    //std::cout << "LENGTH = " << buffer_str.length() << std::endl;
    std::string vector_str(buffer_data.begin(), buffer_data.end());
    //std::cout << "---BEGINNING OF TEST---" << test << "---END OF TEST---" << std::endl;
    return std::make_pair(vector_str, total_bytes_received);
}

//std::pair<std::string, ssize_t> TcpSession::recv(int client) const {
//    std::string buffer_str;
//    char        buffer[READ_BUFFER_SIZE + 1];
//    ssize_t     bytes_received;
//    ssize_t     total_bytes_received = 0;
//
//    try {
//        do {
//            bytes_received         = ::recv(client, buffer, READ_BUFFER_SIZE, 0);
//            buffer[bytes_received] = '\0';
//            buffer_str.append(buffer, bytes_received);
//            total_bytes_received += bytes_received;
//        } while (bytes_received == READ_BUFFER_SIZE);
//    }
//    catch (std::exception &e) {
//        std::cerr << "THAT INFAMOUS STRING LENGTH ERROR!" << std::endl;
//    }
//
//    if (bytes_received == -1) {
//        Logger::instance().log("Error: Failed to receive from socket");
//    }
//
//    return std::make_pair(buffer_str, total_bytes_received);
//}

Session* tcp_session_generator(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    return new TcpSession(sockfd, addr, addrlen);
}

Socket::Socket(SessionGenerator session_generator) : session_generator_(session_generator) {}

Socket::~Socket() {}

TcpSocket::TcpSocket(SessionGenerator session_generator) : Socket(session_generator) {
    /**
     * @todo flags (isntead of 0) - these should be in setsockopt()
     *  SOCK_NONBLOCK? There's no define for it, O_NONBLOCK is a flag for open()
     *  SO_DEBUG might be useful
     *  SO_REUSEADDR might be useful
     *  SO_KEEPALIVE might be useful
     */
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ == -1) {
        Logger::instance().log("Error: Failed to create socket");
    }
    if (fcntl(sockfd_, F_SETFL, O_NONBLOCK) == -1) {
        Logger::instance().log("Error: Failed to set socket to non-blocking");
    }
}

TcpSocket::~TcpSocket() {}

int TcpSocket::bind(std::string addr, int port) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;  // AF_INET indicates IPv4
    hints.ai_socktype = SOCK_STREAM;

    // Convert the port number to a string because getaddrinfo needs it that way
    char port_str[6];  // max port number is 65535, 5 characters + null terminator
    snprintf(port_str, sizeof(port_str), "%d", port);

    int status = getaddrinfo(addr.c_str(), port_str, &hints, &res);
    if (status != 0) {
        Logger::instance().log(std::string("Error: getaddrinfo failed with error: ") +
                               gai_strerror(status));
    }

    // Copy the address information to addr_in_
    memcpy(&addr_in_, res->ai_addr, sizeof(addr_in_));

    // Binds socket to an address and port
    if (::bind(sockfd_, (struct sockaddr*)&addr_in_, sizeof(addr_in_)) == -1) {
        Logger::instance().log("Error: Failed to bind socket -> " + std::string(strerror(errno)));
    }

    freeaddrinfo(res);
    return sockfd_;
}

void TcpSocket::listen() {
    // Sets server to listen passively
    if (::listen(sockfd_, SO_MAX_QUEUE) == -1) {
        Logger::instance().log("Error: Failed to listen on socket -> " + std::string(strerror(errno)));
    }
}

Session* TcpSocket::accept() {
    struct sockaddr* client_addr     = new struct sockaddr;
    socklen_t        client_addr_len = sizeof(sockaddr);

    int client_sockfd = ::accept(sockfd_, client_addr, &client_addr_len);
    if (client_sockfd == -1) {
        delete client_addr;
        Logger::instance().log("Error: Failed to accept connection -> " + std::string(strerror(errno)));
    }

    return session_generator_(client_sockfd, client_addr, client_addr_len);
}

void TcpSocket::close() {
    if (::close(sockfd_) == -1) {
        Logger::instance().log("Error: Failed to close socket -> " + std::string(strerror(errno)));
    }
}

Socket* tcp_socket_generator() {
    return new TcpSocket();
}
