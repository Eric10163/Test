#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <iostream>
#include <string>

typedef websocketpp::client<websocketpp::config::asio_client> client;
using websocketpp::lib::bind;

class websocket_endpoint {
	public:
		websocket_endpoint () {
			m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
			m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

			m_endpoint.init_asio();
			m_endpoint.start_perpetual();

			m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));
		}
		

		int connect(std::string const & uri) {
			websocketpp::lib::error_code ec;
			client::connection_ptr con = m_endpoint.get_connection(uri, ec);

			if (ec) {
				std::cout << "> Connect initialization error: " << ec.message() << std::endl;
				return -1;
			}
			
			m_hdl = con->get_handle();
	        	m_endpoint.connect(con);
			return 1;
    		}
		
		void send(std::string message) {
			websocketpp::lib::error_code ec;
        
			m_endpoint.send(m_hdl, message, websocketpp::frame::opcode::text, ec);
			if (ec) {
				std::cout << "> Error sending message: " << ec.message() << std::endl;
				return;
			}
		}

		
	private:
		client m_endpoint;
		websocketpp::connection_hdl m_hdl;
		websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
};

int main() {
	bool done = false;
	std::string input;
	websocket_endpoint endpoint;
	int id = endpoint.connect("ws://127.0.0.1:9002");

	while (!done) {
		std::cout << "Enter Command: ";
		std::getline(std::cin, input);

		static std::string message;
		endpoint.send(input);
	}

	return 0;
}
