/**
 * @file socket.hpp
 * @author lee kil hun (gns2.lee)
 * @brief
 * @version 0.1
 *         2024.06.15  add socket timeout
 * @date 2024-04-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#ifndef _AP_COMMON___INC_HW_SOCKET_HPP_
#define _AP_COMMON___INC_HW_SOCKET_HPP_

#include "ap_def.hpp"

#pragma comment(lib, "ws2_32.lib")
#define LOOPBACK_LOCALHOST_IP "127.0.0.1"

#ifdef _USE_HW_SOCKET

namespace apcr
{

  using Pool_t = std::pmr::monotonic_buffer_resource;

  enum class NetMode_e : int
  {
    TCP_Server,
    TCP_Client,
    UDP_Server,
    UDP_Client,
  };

  // Concept definition
  template <typename ObjectType>
  concept has_socket_mode_c = requires(ObjectType Obj) {
    typename ObjectType::SocketMode;
  };

  class TcpClient
  {
  public:
    using SocketMode = NetMode_e;
    SocketMode socket_mode;
    SOCKET     m_socket;

  private:
    bool             m_isConnected;
    struct addrinfo *m_addr;
    std::string      m_ip;
    int              m_port;

  public:
    TcpClient() :
    socket_mode{NetMode_e::TCP_Client},
    m_socket{},
    m_isConnected{},
    m_addr{},
    m_ip{},
    m_port{}
    {
    }

    TcpClient(const std::string &ip_str, int port)
    {
      socket_mode = NetMode_e::TCP_Client;
      InitSocket(ip_str, port);
    }

    virtual ~TcpClient()
    {
      // 소켓을 닫습니다.
      ::freeaddrinfo(m_addr);
      ::closesocket(m_socket);
      // WSACleanup() 함수를 호출하여 Winsock 라이브러리를 정리합니다.
      WSACleanup();
    }

  private:
    void initSocket(const std::string &ip_str, int port)
    {
      m_ip   = ip_str;
      m_port = port;
      WSADATA wsaData{};
      if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // version 2.2
        throw std::runtime_error("gError: Failed to initialize Winsock");

      std::string version{};
      version = "version " + std::to_string(HIBYTE(wsaData.wVersion)) + "." + std::to_string(LOBYTE(wsaData.wVersion));
      std::cout << "> wsaData.wVersion : " << version << std::endl;
      version = "version " + std::to_string(HIBYTE(wsaData.wHighVersion)) + "." + std::to_string(LOBYTE(wsaData.wHighVersion));
      std::cout << "> wsaData.wHighVersion : " << version << std::endl;
      std::cout << "> wsaData.szSystemStatus : " << wsaData.szSystemStatus << std::endl;

      struct addrinfo hints = {};
      hints.ai_family       = AF_INET;
      hints.ai_socktype     = SOCK_STREAM;
      hints.ai_flags        = AI_PASSIVE;
      int status            = getaddrinfo(ip_str.c_str(), std::to_string(port).c_str(), &hints, &m_addr);
      if (status != 0)
      {
        throw std::runtime_error("getaddrinfo failed: "); //+ std::string(gai_strerror(status))
      }
      m_socket = socket(m_addr->ai_family, m_addr->ai_socktype, m_addr->ai_protocol);
      if (m_socket == INVALID_SOCKET)
      {
        throw std::runtime_error("socket creation failed: " + std::to_string(WSAGetLastError()));
      }
      // set timeout
      struct timeval tv;
      tv.tv_sec  = 1; // 1sec
      tv.tv_usec = 0;
      setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
      setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));
    }

    void connect(const std::string &ip, int port)
    {
      if (::connect(m_socket, m_addr->ai_addr, (int)m_addr->ai_addrlen) == SOCKET_ERROR)
      {
        ::closesocket(m_socket);
        throw std::runtime_error("connection failed: " + std::to_string(WSAGetLastError()));
      }
    }

    void closeSocket()
    {
      ::closesocket(m_socket);
      m_isConnected = false;
    }

    int send(const std::vector<char> &datas)
    {
      if (!m_isConnected)
        return -1;
      return ::send(m_socket, datas.data(), (int)datas.size(), 0);
    }

    int receive(std::vector<char> &ret_vec)
    {
      char buffer[1024];
      int  ret_flag{};
      int  bytesReceived = ::recv(m_socket, buffer, sizeof(buffer), ret_flag);
      if (bytesReceived)
      {
        {
          // TODO::
        }

        for (int i = 0; i < bytesReceived; ++i)
        {
          ret_vec.emplace_back(buffer[i]);
        }
        ret_vec.shrink_to_fit();
      }
      return bytesReceived;
    }

    int receive(std::pmr::vector<char> &pool_vec)
    {
      int ret_flag{};
      int bytesReceived = ::recv(m_socket, pool_vec.data(), sizeof(pool_vec.capacity()), ret_flag); // waiting block
      if (bytesReceived)
      {
        {
          // TODO::
        }
      }

      return (int)pool_vec.size();
    }

  public:
    bool InitSocket(const std::string &ip_str, int port)
    {
      try
      {
        initSocket(ip_str, port);
        return true;
      }
      catch (std::exception &e)
      {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
      }

      return true;
    }

    bool Connect()
    {
      if (m_ip.empty() && m_port == 0)
        return false;
      try
      {
        connect(m_ip, m_port);
        m_isConnected = true;
        return true;
      }
      catch (std::exception &e)
      {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
      }
    }

    //--------------------------------------------------
    //MARK:check server open
    //--------------------------------------------------
    bool check_serverOpen()
    {
      SOCKET &clientSocket = m_socket;

      // 소켓 상태 확인
      std::vector<char> testMsg{
        'p',
        'i',
        'n',
        'g',
      }; //0x01};//
      int result = send(testMsg);

      if (result == SOCKET_ERROR)
      {
        // 서버가 응답하지 않으면 false 반환
        return false;
      }

      return true;
#if 0
      SOCKET &clientSocket = m_socket;
      // 소켓 상태 확인
      fd_set readSet;
      FD_ZERO(&readSet);
      FD_SET(clientSocket, &readSet);
      // 타임아웃 설정 (5초)
      timeval timeout;
      timeout.tv_sec  = 5;
      timeout.tv_usec = 0;

      // select 함수를 사용하여 소켓 상태 확인
      int result = select(0, &readSet, nullptr, nullptr, &timeout);
      if (result == SOCKET_ERROR)
      {
        //std::cerr << "Select failed: " << WSAGetLastError() << "\n";
        return false;
      }
      else if (result == 0)
      {
        // 타임아웃: 연결이 유지되고 있음
        // std::cout << "Connection is alive.\n";
        return true;
      }
      else
      {
        // 소켓에 문제가 있음 (연결 끊김)
        if (FD_ISSET(clientSocket, &readSet))
        {
          char buffer[1];
          int  bytesReceived = recv(clientSocket, buffer, sizeof(buffer), MSG_PEEK);
          if (bytesReceived == 0)
          {
            //std::cout << "Connection closed by server.\n";
            return false;
          }
          else if (bytesReceived == SOCKET_ERROR)
          {
            //std::cerr << "Connection error: " << WSAGetLastError() << "\n";
            return false;
          }
        }
      }
      return false;
#endif
    }

    void Close()
    {
      closeSocket();
    }

    template <typename DataType>
    int Send(const DataType &datas)
    {
      if constexpr (std::is_same_v<DataType, std::vector<char>>)
      {
        // std::cout << "Send Vector Type Datas" << std::endl;
        return send(datas);
      }
      else if constexpr (std::is_same_v<DataType, std::string>)
      {
        // std::cout << "Send String Type Datas" << datas << std::endl;
        return send(std::vector<char>{datas.begin(), datas.end()});
      }
      else
      {
        using type_of = decltype(datas);
        std::cout << "Send Data Fail!" << std::endl;
        return -1;
      }
    }

    template <typename DataType>
    void Receive(DataType &vbuf) // std::vector<char>
    {
      if constexpr (std::is_same_v<DataType, std::vector<char>>)
      {
        int rec_length = receive(vbuf);
      }
      else if constexpr (std::is_same_v<DataType, std::pmr::vector<char>>)
      {
        int rec_length = receive(vbuf);
      }
    }
  };

  // end of   class TcpClient
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class TcpServer
  {
  public:
    using SocketMode = NetMode_e;
    SocketMode socket_mode;
    SOCKET     m_socket;

  private:
    struct st_client
    {
      sockaddr_storage their_addr{};
      SOCKET           client_socket{};
    } m_clientSocket;

    bool                               m_isOpened;
    bool                               m_isConnectedClient;
    struct addrinfo                   *m_addr;
    std::string                        m_ip;
    int                                m_port;
    std::function<int(void *, void *)> m_cb{};
    TinyC::Que<std::string>            m_Que{1024};

  public:
    TcpServer() :
    m_clientSocket{},
    socket_mode{NetMode_e::TCP_Server},
    m_socket{},
    m_isOpened{},
    m_isConnectedClient{},
    m_addr{},
    m_ip{},
    m_port{}
    {
    }

    TcpServer(const std::string &ip_str, int port)
    {
      socket_mode = NetMode_e::TCP_Server;
      initSocket(ip_str, port);
    }

    virtual ~TcpServer()
    {
      ::freeaddrinfo(m_addr);
      ::closesocket(m_socket);
      ::WSACleanup();
    }

  private:
    void initSocket(const std::string &ip_str, int port)
    {
      WSADATA wsaData{};
      m_port = port;
      if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // version 2.2
        throw std::runtime_error("gError: Failed to initialize Winsock");

      std::string version{};
      version = "version " + std::to_string(HIBYTE(wsaData.wVersion)) + "." + std::to_string(LOBYTE(wsaData.wVersion));
      std::cout << "\t> wsaData.wVersion : " << version << std::endl;
      version = "version " + std::to_string(HIBYTE(wsaData.wHighVersion)) + "." + std::to_string(LOBYTE(wsaData.wHighVersion));
      std::cout << "\t> wsaData.wHighVersion : " << version << std::endl;
      std::cout << "\t> wsaData.szSystemStatus : " << wsaData.szSystemStatus << std::endl;

      struct addrinfo hints = {};
      hints.ai_family       = AF_INET;
      hints.ai_socktype     = SOCK_STREAM;
      hints.ai_flags        = AI_PASSIVE;
      int status            = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &m_addr);
      if (status != 0)
      {
        throw std::runtime_error("getaddrinfo failed: "); // + std::string(gai_strerror(status))  //unicode set.
      }
      m_socket = socket(m_addr->ai_family, m_addr->ai_socktype, m_addr->ai_protocol);
      if (m_socket == INVALID_SOCKET)
      {
        throw std::runtime_error("socket creation failed: " + std::to_string(WSAGetLastError()));
      }
      //////////////////////////////////
    }

    void bindListen()
    {
      if (bind(m_socket, m_addr->ai_addr, (int)m_addr->ai_addrlen) == SOCKET_ERROR)
      {
        closesocket(m_socket);
        throw std::runtime_error("bind failed: " + std::to_string(WSAGetLastError()));
      }

      freeaddrinfo(m_addr);

      if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
      {
        closesocket(m_socket);
        throw std::runtime_error("listen failed: " + std::to_string(WSAGetLastError()));
      }
    }

    int send(const std::vector<char> &datas)
    {
      if (!m_isConnectedClient)
        return -1;
      return (::send(m_clientSocket.client_socket, datas.data(), (int)datas.size(), 0) == SOCKET_ERROR);
    }

    int receive(std::vector<char> &ret_vec)
    {
      int ret_flag{};
      if (m_Que.Available())
      {
        std::string rec_data{};
        m_Que.Get(&rec_data);

        for (size_t i = 0; i < rec_data.length(); ++i)
        {
          ret_flag++;
          ret_vec.emplace_back((char)rec_data[i]);
        }
        ret_vec.shrink_to_fit();
      }
      return ret_flag;
    }

    int receive(std::pmr::vector<char> &pool_vec)
    {
      int ret_flag{};
      if (m_Que.Available())
      {
        std::string rec_data{};
        m_Que.Get(&rec_data);

        for (size_t i = 0; i < rec_data.length(); ++i)
        {
          ret_flag++;
          pool_vec.emplace_back((char)rec_data[i]);
        }
      }

      return ret_flag;
    }


  public:
    bool InitSocket(const std::string &ip_str, int port)
    {
      // Winsock 라이브러리를 초기화합니다.

      try
      {
        initSocket(ip_str, port);
        return true;
      }
      catch (std::exception &e)
      {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
      }

      return true;
    }

    bool OpenServer()
    {
      try
      {
        bindListen();
        std::cout << "\t> waiting for connection..." << std::endl;
        m_isOpened = true;
        return true;
      }
      catch (std::exception &e)
      {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
      }
    }

    void Run()
    {
      socklen_t addr_size = sizeof(m_clientSocket.their_addr);
      bool      run_recovery{};
      while (run_recovery == false)
      {
        std::cout << "\t> wait accept port " << std::endl;
        m_clientSocket.client_socket = ::accept(m_socket, (sockaddr *)&m_clientSocket.their_addr, &addr_size);
        if (m_clientSocket.client_socket == INVALID_SOCKET)
        {
          throw std::runtime_error("accept failed: " + std::to_string(WSAGetLastError()));
        }
        m_isConnectedClient = true;
        while (true)
        {
          std::cout << "\t> wait data receive" << std::endl;
          char buffer[1024];
          int  num_bytes = ::recv(m_clientSocket.client_socket, buffer, 1024, 0);
          if (num_bytes == SOCKET_ERROR)
          {
            std::cout << "client connection broken" << std::endl;
            tim::delay(1000);
            run_recovery = true;
            break;
            //throw std::runtime_error("recv failed: " + std::to_string(WSAGetLastError()));
          }
          if (num_bytes == 0)
          {
            std::cout << "client disconnected" << std::endl;
            break;
          }
          std::string recv_data(buffer, num_bytes);
          {
            if (m_cb)
            {
              int size = (int)recv_data.size();
              m_cb(&size, recv_data.data());
            }
            else
            {
              m_Que.Put(recv_data);
            }
          }
          std::cout << "\t> data received : " << recv_data << std::endl;
        }
        // while msg
        m_isConnectedClient = false;

        ::closesocket(m_clientSocket.client_socket);
      }
      // while (m_isOpened)
      if (run_recovery)
      {
        // TODO::
      }
    }

    inline void registerCallback(std::function<int(void *, void *)> cb)
    {
      m_cb = cb;
    }

    bool IsClientConnected() const
    {
      return m_isConnectedClient;
    }

    void Close()
    {
      m_isOpened = false;

      ::shutdown(m_socket, SD_BOTH);
      ::freeaddrinfo(m_addr);
      ::closesocket(m_socket);
      ::WSACleanup();
    }

    template <typename DataType>
    int Send(const DataType &datas)
    {
      if constexpr (std::is_same_v<DataType, std::vector<char>>)
      {
        return send(datas);
      }
      else if constexpr (std::is_same_v<DataType, std::string>)
      {
        return send(std::vector<char>{datas.begin(), datas.end()});
      }
      else
      {
        using type_of = decltype(datas);
        std::cout << "Send Data Fail!" << std::endl;
        return -1;
      }
    }

    template <typename DataType>
    void Receive(DataType &vbuf) // std::vector<char>
    {
      if constexpr (std::is_same_v<DataType, std::vector<char>>)
      {
        int rec_length = receive(vbuf);
      }
      else if constexpr (std::is_same_v<DataType, std::pmr::vector<char>>)
      {
        int rec_length = receive(vbuf);
      }
    }
  };

  // end of   class TcpServer

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class UdpClient
  {
  public:
    using SocketMode = NetMode_e;
    SocketMode socket_mode;
    SOCKET     m_socket;

  private:
    bool               m_isConnected;
    struct sockaddr_in m_addr;
    std::string        m_ip;
    int                m_port;

  public:
    UdpClient() :
    socket_mode{NetMode_e::UDP_Client},
    m_socket(),
    m_isConnected{},
    m_addr{},
    m_ip{},
    m_port{}
    {
    }

    UdpClient(const std::string &host, int port)
    {
      socket_mode = NetMode_e::UDP_Client;
      initSocket(host, port);
    }

    ~UdpClient()
    {
      // 소켓을 닫습니다.
      ::closesocket(m_socket);
      // WSACleanup() 함수를 호출하여 Winsock 라이브러리를 정리합니다.
      WSACleanup();
    }

  private:
    void initSocket(const std::string &ip_str, int port)
    {
      m_ip   = ip_str;
      m_port = port;
      WSADATA wsaData{};
      if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // version 2.2
        throw std::runtime_error("gError: Failed to initialize Winsock");

      m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (m_socket == INVALID_SOCKET)
      {
        WSACleanup();
        throw std::runtime_error("socket creation failed: " + std::to_string(WSAGetLastError()));
      }
    }

    void connect(const std::string &ip, int port)
    {
      memset(&m_addr, 0, sizeof(m_addr));
      m_addr.sin_family = AF_INET;
      m_addr.sin_port   = ::htons(port);

      if (::inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr) == SOCKET_ERROR)
      {
        ::closesocket(m_socket);
        throw std::runtime_error("connection failed: " + std::to_string(WSAGetLastError()));
      }
      m_isConnected = true;
    }

    void closeSocket()
    {
      ::closesocket(m_socket);
      m_isConnected = false;
      WSACleanup();
    }

    int send(const std::vector<char> &datas)
    {
      if (!m_isConnected)
        return -1;
      // 데이터를 송신합니다.
      return (::sendto(m_socket, datas.data(), (int)datas.size(), 0, (struct sockaddr *)&m_addr, sizeof(m_addr)) == SOCKET_ERROR);
    }

    int receive(std::vector<char> &ret_vec)
    {
      char buffer[1024];
      int  ret_flag{};
      int  size          = sizeof(m_addr);
      int  bytesReceived = ::recvfrom(m_socket, buffer, 1024, 0, (struct sockaddr *)&m_addr, &size);
      if (bytesReceived)
      {
        for (int i = 0; i < bytesReceived; ++i)
        {
          ret_vec.emplace_back(buffer[i]);
        }
        ret_vec.shrink_to_fit();
      }
      return bytesReceived;
    }

  public:
    bool InitSocket(const std::string &ip_str, int port)
    {
      // Winsock 라이브러리를 초기화합니다.
      try
      {
        initSocket(ip_str, port);
        return true;
      }
      catch (std::exception &e)
      {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
      }

      return true;
    }

    bool Connect()
    {
      if (m_ip.empty() && m_port == 0)
        return false;
      try
      {
        connect(m_ip, m_port);
        return true;
      }
      catch (std::exception &e)
      {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
      }
    }

    void Close()
    {
      closeSocket();
    }

    template <typename DataType>
    int Send(const DataType &datas)
    {
      int ret{};
      if constexpr (std::is_same_v<DataType, std::vector<char>>)
      {
        // std::cout << "Send Vector Type Datas" << std::endl;
        ret = send(datas);
        if (ret == ERROR_SUCCESS)
          ret = (int)datas.size();
        return ret;
      }
      else if constexpr (std::is_same_v<DataType, std::string>)
      {
        // std::cout << "Send String Type Datas" << datas << std::endl;
        ret = send(std::vector<char>{datas.begin(), datas.end()});
        if (ret == ERROR_SUCCESS)
          ret = (int)datas.size();
        return ret;
      }
      else
      {
        using type_of = decltype(datas);
        std::cout << "Send Data Fail!" << std::endl;
        return -1;
      }
    }

    template <typename DataType>
    void Receive(DataType &vbuf) // std::vector<char>
    {
      if constexpr (std::is_same_v<DataType, std::vector<char>>)
      {
        /*
                 constexpr std::size_t Count = 1024;
                 constexpr std::size_t ByteCount = sizeof(char) * Count + 1024;
                 char buffer[ByteCount]{};         // stack memory
                 Pool_t pool{ buffer, ByteCount }; // std::pmr::monotonic_buffer_resource
                 */
        int rec_length = receive(vbuf);
      }
      else if constexpr (std::is_same_v<DataType, std::pmr::vector<char>>)
      {
        int rec_length = receive(vbuf);
      }
    }
  };

  // end of UdpClient

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class UdpServer
  {
  public:
    using SocketMode = NetMode_e;
    SocketMode socket_mode;
    SOCKET     m_socket;

  private:
    bool                               m_isOpened;
    sockaddr_in                        m_addr;
    sockaddr_in                        m_their_addr;
    std::string                        m_ip;
    int                                m_port;
    std::function<int(void *, void *)> m_cb{};
    TinyC::Que<std::string>            m_Que{1024};

  public:
    UdpServer() :
    socket_mode{NetMode_e::UDP_Server},
    m_socket(),
    m_isOpened{},
    m_addr{},
    m_their_addr{},
    m_ip{},
    m_port{}
    {
    }

    UdpServer(const std::string &ip_str, int port)
    {
      socket_mode = NetMode_e::UDP_Server;
      initSocket(ip_str, port);
    }

    ~UdpServer()
    {
      // Clean up
      ::closesocket(m_socket);
      WSACleanup();
    }

  private:
    void initSocket(const std::string &ip_str, int port)
    {
      WSADATA wsaData{};
      m_port = port;
      if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // version 2.2
        throw std::runtime_error("gError: Failed to initialize Winsock");

      std::string version{};
      version = "version " + std::to_string(HIBYTE(wsaData.wVersion)) + "." + std::to_string(LOBYTE(wsaData.wVersion));
      std::cout << "\t> wsaData.wVersion : " << version << std::endl;
      version = "version " + std::to_string(HIBYTE(wsaData.wHighVersion)) + "." + std::to_string(LOBYTE(wsaData.wHighVersion));
      std::cout << "\t> wsaData.wHighVersion : " << version << std::endl;
      std::cout << "\t> wsaData.szSystemStatus : " << wsaData.szSystemStatus << std::endl;

      // Create UDP socket
      m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (m_socket == INVALID_SOCKET)
      {
        throw std::runtime_error("socket creation failed: " + std::to_string(WSAGetLastError()));
      }
      //////////////////////////////////
    }

    void bind()
    {
      memset(&m_addr, 0, sizeof(m_addr));
      m_addr.sin_family      = AF_INET;
      m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      m_addr.sin_port        = htons(m_port);

      if (::bind(m_socket, (struct sockaddr *)&m_addr, sizeof(m_addr)) == SOCKET_ERROR)
      {
        ::closesocket(m_socket);
        throw std::runtime_error("bind failed: " + std::to_string(WSAGetLastError()));
      }
    }

    int send(const std::vector<char> &datas)
    {
      if (!m_their_addr.sin_port)
        return -1;
      return (sendto(m_socket, datas.data(), (int)datas.size(), 0, (struct sockaddr *)&m_their_addr, sizeof(m_their_addr)) == SOCKET_ERROR);
    }

    errno_t send(const std::string &data)
    {
      if (!m_isOpened)
        return -1;
      // 데이터를 송신합니다.

      if (sendto(m_socket, data.c_str(), (int)data.length(), 0, (struct sockaddr *)&m_their_addr, sizeof(m_their_addr)) == SOCKET_ERROR)
      {
        std::cerr << "Error sending message" << std::endl;
        return -1;
      }

      return ERROR_SUCCESS;
    }

    int receive(std::vector<char> &ret_vec)
    {
      int ret_flag{};
      if (m_Que.Available())
      {
        std::string rec_data{};
        m_Que.Get(&rec_data);

        for (size_t i = 0; i < rec_data.length(); ++i)
        {
          ret_flag++;
          ret_vec.emplace_back((char)rec_data[i]);
        }
        ret_vec.shrink_to_fit();
      }
      return ret_flag;
    }

    int receive(std::pmr::vector<char> &pool_vec)
    {
      int ret_flag{};
      if (m_Que.Available())
      {
        std::string rec_data{};
        m_Que.Get(&rec_data);

        for (size_t i = 0; i < rec_data.length(); ++i)
        {
          ret_flag++;
          pool_vec.emplace_back((char)rec_data[i]);
        }
      }

      return ret_flag;
    }


  public:
    bool InitSocket(const std::string &ip_str, int port)
    {
      // Winsock 라이브러리를 초기화합니다.

      try
      {
        initSocket(ip_str, port);
        return true;
      }
      catch (std::exception &e)
      {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
      }

      return true;
    }

    bool OpenServer()
    {
      try
      {
        bind();
        std::cout << "bind .." << std::endl;
        m_isOpened = true;

        return true;
      }
      catch (std::exception &e)
      {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
      }
    }

    void Run()
    {
      while (m_isOpened)
      {
        char buffer[1024]{};
        int  client_address_len = sizeof(m_their_addr);

        // Receive log message
        int num_bytes = recvfrom(m_socket, buffer, 1024, 0, (struct sockaddr *)&m_their_addr, &client_address_len);

        if (num_bytes == -1)
        {
          m_their_addr.sin_port = 0;
          std::cerr << "Error receiving message" << std::endl;
        }
        else
        {
          // Print log message to console
          buffer[num_bytes % 1024] = '\0';
          char        addr_str[INET_ADDRSTRLEN]{};
          std::string recv_data(buffer, num_bytes);
          {
            if (m_cb)
            {
              int size = (int)recv_data.size();
              m_cb(&size, recv_data.data());
            }
            else
            {
              m_Que.Put(recv_data);
            }
          }
          std::cout << "\t> data received : " << recv_data << std::endl;

          // std::cout << "Received log message from " << inet_ntop(AF_INET, &m_their_addr.sin_addr, addr_str, INET_ADDRSTRLEN) << ": " << buffer << std::endl;
        }
      }
      // while ()
    }

    inline void registerCallback(std::function<int(void *, void *)> cb)
    {
      m_cb = cb;
    }

    void Close()
    {
      m_isOpened = false;

      ::closesocket(m_socket);
      WSACleanup();
    }

    template <typename DataType>
    int Send(const DataType &datas)
    {
      if constexpr (std::is_same_v<DataType, std::vector<char>>)
      {
        return send(datas);
      }
      else if constexpr (std::is_same_v<DataType, std::string>)
      {
        return send(std::vector<char>{datas.begin(), datas.end()});
      }
      else
      {
        using type_of = decltype(datas);
        std::cout << "Send Data Fail!" << std::endl;
        return -1;
      }
    }

    template <typename DataType>
    void Receive(DataType &vbuf) // std::vector<char>
    {
      if constexpr (std::is_same_v<DataType, std::vector<char>>)
      {
        int rec_length = receive(vbuf);
      }
      else if constexpr (std::is_same_v<DataType, std::pmr::vector<char>>)
      {
        int rec_length = receive(vbuf);
      }
    }
  };

  // /end of   class UdpServer

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

// end of namespace apcr

namespace apcr
{

  template <typename ClassType>
    requires has_socket_mode_c<ClassType>
  class NetComm : public IComm
  {
  public:
    struct cfg_t
    {
      std::string ip_str{};
      uint16_t    port_no{};

      cfg_t() = default;
      cfg_t(std::string &str, uint32_t p) :
      ip_str{str},
      port_no{(uint16_t)p} {};

      ~cfg_t()                             = default;
      cfg_t(const cfg_t &other)            = default; // copy constructor
      cfg_t &operator=(const cfg_t &other) = default; // copy assignment
      cfg_t(cfg_t &&other)                 = default; // move constructor
      cfg_t &operator=(cfg_t &&other)      = default; // move assignment
    };

  public:
    cfg_t m_cfg{};
    ClassType::SocketMode m_SocketMode{ClassType{}.socket_mode};

  private:
    /* data */
    // TinyC::Que<uint8_t> m_Que;
    using type_t = ClassType;
    std::unique_ptr<ClassType>         m_instance{std::make_unique<ClassType>()};
    bool                               m_isOpenPort{};
    bool                               m_isBlocked{};
    std::thread                        m_trd{};
    std::atomic<bool>                  m_liftThread{};
    std::function<int(void *, void *)> m_cb{};
    uint32_t                           pre_ms{};
    std::string                        m_lastlog{};

  public:
    NetComm(const apcr::attribute_st &f);
    NetComm() = default;

    ~NetComm();

  public:
    /****************************************************
      *  overriding
      ****************************************************/
    bool write_log(TinyC::Log::level level, apcr::log_info_t log, const char *fmt, ...) override;

  private:
    bool recovery();
    void threadStop();
    void threadJob();
    void threadRun();
  public:
    std::string GetMode() const;

    // inline GetSocket(*SOCKET ret_sckt)
    // {
    //   return &m_instance->m_socket;
    // }


    bool InitSocket(cfg_t cfg);
    bool Connect(bool is_block = false);
    bool IsBlocked() const;

    /****************************************************
      *  IComm overriding
      ****************************************************/
    errno_t Open() override;
    void Close() override;
    bool IsConnected() const override;
    void registerCallback(std::function<int(void *, void *)> cb) override;
    int SendData(const char *ptr_data, size_t length) override;
    int RecvData(std::vector<char> &rev_v) override;


#ifdef _USE_HW_CLI
    /****************************************************
     *	test code
     ****************************************************/
    int testCB_func(int argc, char *argv[]);
#endif
  };

  // end of class NetComm


}

// end of namespace apcr

#endif
// end fo _USE_HW_SOCKET


#endif // _AP_COMMON___INC_HW_SOCKET_HPP_