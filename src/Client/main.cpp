#include <iostream>
#include "ChatClient.hpp"

int main() {
	ChatClient client;

	client.connect_to_server();

	std::cout << "Client";
}