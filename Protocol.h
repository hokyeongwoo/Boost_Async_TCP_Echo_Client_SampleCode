#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

using boost::asio::ip::tcp;

const std::string _MY_IP("10.1.28.29");


class CProtocol
{
private:

	enum EEnum
	{
		eBuffSize = 128,
	};

	tcp::socket m_Socket;
	bool m_bConnect;

	int m_nTestCount;

public:

	CProtocol(boost::asio::io_service& io) : m_Socket(io)
	{
		m_bConnect = false;
		m_nTestCount = 0;
	}

	~CProtocol() {}

	void Connect()
	{
		// 입력 받은 host을 resolving한다. 
		tcp::resolver resolver(m_Socket.get_io_service());
		tcp::resolver::query query(_MY_IP, "daytime");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;

		// resolving된 endpoint로 접속을 시도한다. 
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			m_Socket.close();
			m_Socket.connect(*endpoint_iterator++, error);
		}

		// 접속 실패인지 확인 
		if (error)
			throw boost::system::system_error(error);

		m_bConnect = true;

		// 읽어올 데이터를 저장할 array를 만든다. 
		boost::array<CHAR, eBuffSize> buf;
		size_t len = m_Socket.read_some(boost::asio::buffer(buf), error);
		if (error == boost::asio::error::eof)
		{
			m_bConnect = false;
			return;
		}
		else if (error)
			throw boost::system::system_error(error);

		// 받은 데이터를 cout로 출력한다. 
		std::cout.write(buf.data(), len);
	}

	bool IsRun() { return m_bConnect; }
	bool IsSocketOpen()
	{
		if (!m_Socket.is_open() && m_bConnect)   // 커넥트 된 이후 소켓이 닫혀버렸다면 
		{
			m_bConnect = false;                 // 커넥트도 끊김 판정 
			return false;
		}

		return true;
	}

	void handle_Recive()
	{
		while (m_bConnect)
		{
			if (!IsSocketOpen())
				break;

			try
			{
				boost::array<BYTE, eBuffSize> buf;
				int len = m_Socket.receive(boost::asio::buffer(buf));
				if (len > 0)
				{
					std::cout << "> Recv(len " << len << ") ";
					std::cout.write((CHAR*)buf.data(), len) << std::endl;
				}
			}
			catch (std::exception& e)
			{
				m_bConnect = false;
				std::cerr << e.what() << std::endl;
			}
		}

	}

	void handle_Send()
	{
		while (m_bConnect)
		{
			if (!IsSocketOpen())
				break;

			try
			{
				boost::array<BYTE, eBuffSize> buf = { boost::lexical_cast<BYTE>(m_nTestCount) };
				boost::system::error_code error;
				int len = boost::asio::write(m_Socket, boost::asio::buffer(buf, buf.size()), error);
				if (len > 0)
					std::cout << "> Send " << m_nTestCount << std::endl;
				m_nTestCount++;
			}
			catch (std::exception& e)
			{
				m_bConnect = false;
				std::cerr << e.what() << std::endl;
			}

			Sleep(3000);
		}
	}
};