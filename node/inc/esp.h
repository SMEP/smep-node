#ifndef ESP_H_
#define ESP_H_


#include "WString.h"
#include <unordered_map>

typedef std::unordered_map<String, String, StringHasher> PostParameters;

class StrCmd;

class ESP8266 {
public:
	ESP8266();
	~ESP8266(void);


	//AP Related
	void ConnectAP( char * SSID, char * PW );
	void GetClientIP(String& ip) ;
	bool IsConnected();
	void ListAP();
	void DisconnectAP();


	// ESP Specifics
	void StartServer(uint16_t port);
	void SetMux(bool state);
	void SetBaud(uint32_t baud);
	void Reset();

	// ESP cmds/data routines
	bool SendData( const uint8_t* data, const uint16_t size, const int timeout, String* ret);
	bool SendData( const char * data, const int timeout );
	bool SendData( const String& cmd,  const int timeout );
	bool SendData( StrCmd& cmd, const int timeout, String* ret = NULL );
	bool SendData( const String& cmd, const int timeout, String* ret );
	bool ReadResponse(String& resp, const int timeout );


	// HTTP Related
	void SendHTTPGet( const String& parameters, const String& ip, int port );
	void SendHTTPPost(const String& ip, int port, const String& path, const String& parameters );
	void SendHTTPPost(const String& ip, int port, const String& path, const PostParameters& parameters );

	// TCP/UDP Related
	bool CloseConnection(uint8_t id);
	void TCPStartCon(const uint8_t conId, const String& ip, int port );
	void TCPSendData( uint8_t conId, const String& data );

	void StartListeningBroadcast(String& broadcastIP, uint16_t broadcastPort);
	bool CreateUDPConnection(uint8_t id, const String& ip, uint16_t localPort, uint16_t remotePort );
	bool SendUDPPacket(uint8_t conID, uint8_t* packet, uint16_t size);



	// Application level

};

//typedef String StrCmd; // TODO: Test if it would work
class StrCmd : public String {

public:
	StrCmd(const char *cstr = "") : String(cstr) {}
	StrCmd(const String &str) : String(str) {}


	~StrCmd(void) {}
};

#endif
