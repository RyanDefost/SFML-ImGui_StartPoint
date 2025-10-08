#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <vector>

struct Client {
	std::string username;
	std::string identity;
	std::chrono::steady_clock::time_point last_ping;
	bool authenticated = false;
};

class ChatServer {
private:
	zmq::context_t context;
	zmq::socket_t socket;
	std::unordered_map<std::string, Client> clients; // identity -> client
	std::unordered_map<std::string, std::string> usernames; // username -> identity

public:
	ChatServer(int port) : context(1), socket(context, zmq::socket_type::router) {
		socket.bind("tcp://*:" + std::to_string(port));
		std::cout << "Chat server listening on port " << port << std::endl;
	}

	void run() {
		std::thread ping_thread(&ChatServer::ping_clients, this);
		while (true) {
			try {
				zmq::multipart_t msg;
				if (msg.recv(socket, ZMQ_DONTWAIT)) {
					handle_message(msg);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			catch (const zmq::error_t& e) {
				std::cerr << "Error: " << e.what() << std::endl;
			}
		}
		ping_thread.join();
	}
private:
void handle_message(zmq::multipart_t& msg) {
	if (msg.size() < 2) return;
	std::string identity = msg[0].to_string();
	std::string msg_type = msg[1].to_string();
	// Update last seen time
	if (clients.find(identity) != clients.end()) {
		clients[identity].last_ping = std::chrono::steady_clock::now();
	}
	if (msg_type == "CONNECT") {
		handle_connect(identity);
	}
	else if (msg_type == "PONG") {
		handle_pong(identity);
	}
	else if (msg_type == "SET_USERNAME" && msg.size() >= 3) {
		handle_set_username(identity, msg[2].to_string());
	}
	else if (msg_type == "PRIVATE_MSG" && msg.size() >= 4) {
		handle_private_message(identity, msg[2].to_string(),
			msg[3].to_string());
	}
	else if (msg_type == "PUBLIC_MSG" && msg.size() >= 3) {
		handle_public_message(identity, msg[2].to_string());
	}
	else if (msg_type == "LEFT") {
		handle_left_message(identity);
		disconnect_client(identity);
	}
}

void handle_connect(const std::string& identity) {
	clients[identity] = Client{
	.username = "",
	.identity = identity,
	.last_ping = std::chrono::steady_clock::now(),
	.authenticated = false
	};
	send_message(identity, "CONNECTED");
	std::cout << "Client connected: " << identity << std::endl;
}
	
void handle_pong(const std::string& identity) {
	if (clients.find(identity) != clients.end()) {
		clients[identity].last_ping = std::chrono::steady_clock::now();
	}
}
	
void handle_set_username(const std::string& identity, const std::string&
	username) {
	if (clients.find(identity) == clients.end()) return;
	// Check if username is already taken
	if (usernames.find(username) != usernames.end() && usernames[username] !=
		identity) {
		send_message(identity, "USERNAME_TAKEN");
		return;
	}
	// Remove old username mapping if exists
	if (!clients[identity].username.empty()) {
		usernames.erase(clients[identity].username);
	}
	// Set new username
	clients[identity].username = username;
	clients[identity].authenticated = true;
	usernames[username] = identity;
	send_message(identity, "USERNAME_SET", username);
	std::cout << "Username set: " << username << " (" << identity << ")" <<
		std::endl;
	for (auto& [target_identity, client] : clients) {
		// Forward message to all
		send_message(target_identity, "JOINED",
			clients[identity].username);
	}
}
	
void handle_private_message(const std::string& sender_identity,
	const std::string& target,
	const std::string& message) {
	if (clients.find(sender_identity) == clients.end() ||
		!clients[sender_identity].authenticated) {
		send_message(sender_identity, "NOT_AUTHENTICATED");
		return;
	}
	std::string target_identity;
	// Check if target is a username or identity
	if (usernames.find(target) != usernames.end()) {
		target_identity = usernames[target];
	}
	else if (clients.find(target) != clients.end()) {
		target_identity = target;
	}
	else {
		send_message(sender_identity, "USER_NOT_FOUND");
		return;
	}
	// Send message to target
	send_message(target_identity, "PRIVATE_MSG",
		clients[sender_identity].username, message);
	// Confirm to sender
	send_message(sender_identity, "MSG_DELIVERED");
	std::cout << "Private message: " << clients[sender_identity].username
		<< " -> " << target << std::endl;
}
	
void handle_public_message(const std::string& sender_identity,
	const std::string& message) {
	if (clients.find(sender_identity) == clients.end() ||
		!clients[sender_identity].authenticated) {
		send_message(sender_identity, "NOT_AUTHENTICATED");
		return;
	}
	for (auto& [identity, client] : clients) {
		// Forward message to all
		send_message(identity, "PUBLIC_MSG",
			clients[sender_identity].username, message);
	}
	std::cout << "Public message: " << clients[sender_identity].username <<
		std::endl;
}
	
void handle_left_message(const std::string& sender_identity) {
	if (clients.find(sender_identity) == clients.end() ||
		!clients[sender_identity].authenticated) {
		send_message(sender_identity, "NOT_AUTHENTICATED");
		return;
	}
	for (auto& [identity, client] : clients) {
		// Forward message to all
		send_message(identity, "LEFT",
			clients[sender_identity].username);
	}
	std::cout << "Left: " << clients[sender_identity].username << std::endl;
}
	
void send_disconnect_message(const std::string& sender_identity) {
	for (auto& [identity, client] : clients) {
		// Forward message to all
		send_message(identity, "DISCONNECTED",
			clients[sender_identity].username);
	}
}
	
void send_message(const std::string& identity, const std::string& msg_type) {
	zmq::multipart_t msg;
	msg.add(zmq::message_t(identity.data(), identity.size()));
	msg.add(zmq::message_t(msg_type.data(), msg_type.size()));
	msg.send(socket);
}
	
void send_message(const std::string& identity, const std::string& msg_type,
	const std::string& data) {
	zmq::multipart_t msg;
	msg.add(zmq::message_t(identity.data(), identity.size()));
	msg.add(zmq::message_t(msg_type.data(), msg_type.size()));
	msg.add(zmq::message_t(data.data(), data.size()));
	msg.send(socket);
}
	
void send_message(const std::string& identity, const std::string& msg_type,
	const std::string& data1, const std::string& data2) {
	zmq::multipart_t msg;
	msg.add(zmq::message_t(identity.data(), identity.size()));
	msg.add(zmq::message_t(msg_type.data(), msg_type.size()));
	msg.add(zmq::message_t(data1.data(), data1.size()));
	msg.add(zmq::message_t(data2.data(), data2.size()));
	msg.send(socket);
}

void ping_clients() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		auto now = std::chrono::steady_clock::now();
		std::vector<std::string> to_disconnect;
		for (auto& [identity, client] : clients) {
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
				now - client.last_ping).count();
			if (elapsed > 4) {
				// Client hasn't responded in 4 seconds, disconnect
				to_disconnect.push_back(identity);
				send_disconnect_message(identity);
			}
			else if (elapsed > 2) {
				// Send ping after 2 seconds of inactivity
				send_message(identity, "PING");
			}
		}
		// Disconnect unresponsive clients
		for (const auto& identity : to_disconnect) {
			disconnect_client(identity);
		}
	}
}
	
void disconnect_client(const std::string& identity) {
	if (clients.find(identity) != clients.end()) {
		if (!clients[identity].username.empty()) {
			usernames.erase(clients[identity].username);
		}
		clients.erase(identity);
		std::cout << "Client disconnected: " << identity << std::endl;
	}
}
};

//int main(int argc, char* argv[]) {
//	int port = 5555;
//	if (argc > 1) {
//		port = std::atoi(argv[1]);
//	}
//	try {
//		ChatServer server(port);
//		server.run();
//	}
//	catch (const std::exception& e) {
//		std::cerr << "Server error: " << e.what() << std::endl;
//		return 1;
//	}
//	return 0;
//}