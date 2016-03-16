#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "Protocol.h"

using boost::asio::ip::tcp;


int main()
{
	try
	{
		boost::asio::io_service io_service; // io_service를 하나 생성한다. 

		CProtocol Ptc(io_service);
		boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
		Ptc.Connect(); // 접속 시도 
		boost::thread Recv(boost::bind(&CProtocol::handle_Recive, &Ptc));
		boost::thread Send(boost::bind(&CProtocol::handle_Send, &Ptc));
		io_service.run();

		while (Ptc.IsRun())
		{
		}

		Recv.join();
		Send.join();

		t.join();   // 쓰레드가 종료될 때까지 메인 함수의 종료를 막는다 
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	int in;
	std::cout << "END";
	std::cin >> in;

	return 0;
}