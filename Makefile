server: server.cpp
	g++ -std=c++17 $< -o $@ -lpistache -lcrypto -lssl -lpthread -lpaho-mqttpp3 -lpaho-mqtt3a