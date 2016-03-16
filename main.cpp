#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "Protocol.h"

using boost::asio::ip::tcp;


int main()
{
	try
	{
		boost::asio::io_service io_service; // io_service�� �ϳ� �����Ѵ�. 

		CProtocol Ptc(io_service);
		boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
		Ptc.Connect(); // ���� �õ� 
		boost::thread Recv(boost::bind(&CProtocol::handle_Recive, &Ptc));
		boost::thread Send(boost::bind(&CProtocol::handle_Send, &Ptc));
		io_service.run();

		while (Ptc.IsRun())
		{
		}

		Recv.join();
		Send.join();

		t.join();   // �����尡 ����� ������ ���� �Լ��� ���Ḧ ���´� 
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