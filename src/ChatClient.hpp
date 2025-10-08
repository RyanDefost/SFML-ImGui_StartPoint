#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

class ChatClient {
private:
	zmq::context_t context;
	zmq::socket_t socket;
	std::atomic<bool> connected{ false };
	std::atomic<bool> authenticated{ false };
	std::atomic<bool> running{ true };
	std::thread receive_thread;
	std::vector<std::string> messages;
	std::string username;
	std::string server_address = "tcp://localhost:5555";
	char username_buffer[64] = "";
	char target_buffer[64] = "";
	char message_buffer[256] = "";
	char server_buffer[128] = "tcp://localhost:5555";
public:
	ChatClient() : context(1), socket(context, zmq::socket_type::dealer) {
		socket.set(zmq::sockopt::linger, 0);
	}
	~ChatClient() {
		running = false;
		if (receive_thread.joinable()) {
			receive_thread.join();
		}
	}
	void connect_to_server() {
		try {
			socket.connect(server_buffer);
			server_address = server_buffer;
			// Send connect message
			send_message("CONNECT");
			// Start receive thread
			receive_thread = std::thread(&ChatClient::receive_loop, this);
			add_message("Connecting to " + server_address + "...");
		}
		catch (const zmq::error_t& e) {
			add_message("Connection failed: " + std::string(e.what()));
		}
	}
	void disconnect() {
		running = false;
		connected = false;
		authenticated = false;
		socket.disconnect(server_address);
		if (receive_thread.joinable()) {
			receive_thread.join();
		}
		add_message("Disconnected from server.");
	}
	void set_username() {
	}
	void send_private_message() {
	}
	void send_public_message() {
	}
	void send_leave_message() {
	}
	void leave() {
		send_leave_message();
	}
	void render_gui() {
		// Connection panel
		ImGui::Begin("Chat Client");
		if (!connected) {
			ImGui::InputText("Server", server_buffer, sizeof(server_buffer));
			if (ImGui::Button("Connect")) {
				connect_to_server();
			}
		}
		else {
			ImGui::Text("Connected to: %s", server_address.c_str());
			ImGui::SameLine();
			if (ImGui::Button("Disconnect")) {
				disconnect();
			}
		}
		ImGui::Separator();
		// Username section
		if (connected && !authenticated) {
			ImGui::Text("Set your username:");
			ImGui::InputText("Username", username_buffer, sizeof(username_buffer));
			if (ImGui::Button("Set Username") && strlen(username_buffer) > 0) {
				set_username();
			}
		}
		else if (authenticated) {
			ImGui::Text("Logged in as: %s", username.c_str());
		}
		ImGui::Separator();
		// Message area
		ImGui::BeginChild("Messages", ImVec2(0, -100), true);
		for (const auto& msg : messages) {
			ImGui::TextWrapped("%s", msg.c_str());
		}
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
			ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		// Send message section
		if (authenticated) {
			ImGui::InputText("Target", target_buffer, sizeof(target_buffer));
			ImGui::InputText("Message", message_buffer, sizeof(message_buffer));
			if (ImGui::Button("Send") || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
				// TODO: Send public & private message
			}
		}
		ImGui::End();
	}
private:
	void send_message(const std::string& msg_type) {
		// TODO: Create multipart_t zmq message, add type, send
	}
	void send_message(const std::string& msg_type, const std::string& data) {
		// TODO: Create multipart_t zmq message, add type & data, send
	}
	void send_message(const std::string& msg_type, const std::string& data1, const
		std::string& data2) {
		// TODO: Create multipart_t zmq message, add type & data1 & data2, send
	}
	void receive_loop() {
		while (running) {
			try {
				zmq::multipart_t msg;
				if (msg.recv(socket, ZMQ_DONTWAIT)) {
					handle_server_message(msg);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			catch (const zmq::error_t& e) {
				if (running) {
					add_message("Receive error: " + std::string(e.what()));
				}
				break;
			}
		}
	}
	void handle_server_message(zmq::multipart_t& msg) {
		if (msg.size() < 1) return;
		std::string msg_type = msg[0].to_string();
		// TODO: Implement messages from server:
		// CONNECTED
		// DISCONNECTED
		// PING
		// USERNAME_SET
		// USERNAME_TAKEN
		// PRIVATE_MSG
		// MSG_DELIVERED
		// USER_NOT_FOUND
		// NOT_AUTHENTICATED
		// PUBLIC_MSG
		// JOINED
		// LEFT
		// Use "add_message(...)" function to put things into the Chat Window
		// TIP: you can read the data out of a msg by msg[1], msg[2], etc., and
		//check the number of parts of the message with msg.size()
	}
	void add_message(const std::string& message) {
		messages.push_back(message);
		if (messages.size() > 100) { // Limit message history
			messages.erase(messages.begin());
		}
	}
};