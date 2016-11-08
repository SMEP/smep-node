#include "esp.h"

#include <cstdio>
#include <ctime>

#include "board.h"

#include "millis.h"
#include "uart.h"


ESP8266::ESP8266() {}
ESP8266::~ESP8266() {}

void ESP8266::ConnectAP( char * SSID, char * PW ) {
	StrCmd cmd = "AT+CWJAP=\"";
	cmd += SSID;
	cmd += "\",\"";
	cmd += PW;
	cmd += "\"";
	SendData( cmd, 10000);
}

void ESP8266::GetClientIP(String& ip) {
	StrCmd cmd = "AT+CIFSR";

	String resp;

	SendData(cmd, 300, &resp);

	uint16_t start = resp.indexOf("+CIFSR:STAIP,\"") + 14;

	String substr = resp.substring( start );

	uint16_t end = substr.indexOf("\"");

	ip = substr.substring(0, end);
}

bool ESP8266::IsConnected() {
	StrCmd cmd = "AT+CWJAP?";

	String resp;

	SendData( cmd, 200, &resp);

	if( resp.indexOf("No AP") )
		return false;

	return true;
}

void ESP8266::ListAP() {
	StrCmd cmd = "AT+CWLAP";
	SendData( cmd, 3000 );
}

void ESP8266::DisconnectAP() {
	StrCmd cmd = "AT+CWQAP";
	SendData( cmd, 500);
}

void ESP8266::StartServer(uint16_t port) {
	StrCmd cmd = "AT+CIPSERVER=1,";
	cmd += port;

	SendData(cmd, 300);
}

void ESP8266::SetMux(bool state) {
	StrCmd cmd = "AT+CIPMUX=";
	cmd += state;
	SendData(cmd, 200);
}


void ESP8266::Reset() {
	StrCmd cmd = "AT+RST";
	SendData(cmd, 100);
}


bool ESP8266::SendData( const uint8_t * data, const uint16_t size, const int timeout, String* ret) {
	bool success;
	String resp;


	UART_Send( data, size );

	success = ReadResponse(resp, timeout);

	Board_UARTPutSTR( (char*)resp.c_str() );

	if( ret != NULL )
		*ret = resp;


	return success;
}

bool ESP8266::SendData(const String& cmd, const int timeout, String* ret ) {
	return SendData( (uint8_t *) cmd.c_str(), cmd.length(), timeout, ret );
}


bool ESP8266::SendData( const String& cmd, const int timeout ) {
	return SendData( cmd, timeout, NULL);
}

bool ESP8266::SendData(const char * data, const int timeout ) {
	String cmd = data;
	return SendData(cmd, timeout);
}

bool ESP8266::SendData( StrCmd& cmd, const int timeout, String* ret ) {
	cmd += "\r\n";
	return SendData( (String&) cmd, timeout, ret);
}

bool ESP8266::ReadResponse(String& resp, const int timeout ) {
	unsigned long int time = Millis();

	bool success = false;

	UART_IntDisable();

	while( (time + timeout) > Millis() ) {
		while( UART_Available() ) {
			resp += (char) UART_Read();

			if( resp.indexOf("OK") ) {
				success = true;
				break;
			} else if( resp.indexOf("FAIL") )
				break;
		}
	}

	UART_IntEnable();

	return success;
}


void ESP8266::SendHTTPGet( const String& parameters, const String& ip, int port ) {
	String content = "GET " + parameters;
	content += " HTTP/1.1\r\nHost: smep.gateway\r\n\r\n";
	content += "\r\n";

	TCPStartCon( 0, ip, port );

	TCPSendData( 0, content );
}

void ESP8266::SendHTTPPost(const String& ip, int port, const String& path, const PostParameters& parameters ) {
	String postParameters = "";

	for( auto const& it : parameters ) {
		postParameters += it.first;
		postParameters += "=";
		postParameters += it.second;
		postParameters += "&";
	}

	postParameters = postParameters.substring(0, postParameters.length()-1); // remove last &

	SendHTTPPost( ip, port, path, postParameters );
}

void ESP8266::SendHTTPPost(const String& ip, int port, const String& path, const String& parameters ) {
	String request = "POST ";
	request += path;
	request += " HTTP/1.1\r\nHost: smep.gateway\r\nContent-Type: application/x-www-form-urlencoded\r\n";
	//request += "Connection: close\r\n";
	request += "Content-Length: ";
	request += parameters.length();
	request += "\r\n\r\n";
	request += parameters;

	TCPStartCon( 0, ip, port );

	TCPSendData( 0, request );

	//CloseConnection( 0 );

}


bool ESP8266::CloseConnection(uint8_t id) {
	StrCmd cmd = "AT+CIPCLOSE=";
	cmd += id;

	return SendData(cmd, 200);
}

void ESP8266::TCPStartCon(const uint8_t conId, const String& ip, int port ) {
	static int i = 0;
	if( i != 0) return;
	StrCmd cmd = "AT+CIPSTART=";
	cmd += conId;
	cmd += ",\"TCP\",\"";
	cmd += ip;
	cmd += "\",";
	cmd += port;

	i++;

	SendData( cmd, 200 );
}

void ESP8266::TCPSendData( uint8_t conId, const String& data ) {
	StrCmd cipSend = "AT+CIPSEND=";
	cipSend += conId;
	cipSend += ",";
	cipSend += data.length();

	SendData(cipSend, 200);
	Delay_ms(20);

	SendData(data, 200);
}


void ESP8266::StartListeningBroadcast(String& broadcastIP, uint16_t broadcastPort) {
	//AT+CIPSTART=4,"UDP","192.168.1.255",55056,3333,0
	//AT+CIPSTART=[ID],[type],[ip],[remotePort],[localPort],[respChange] -- http://4tronix.co.uk/files/ESP8266_AT_Examples_v0_4.pdf

	CreateUDPConnection( 4, broadcastIP, broadcastPort, 1234 ); // remotePort may be random
}

bool ESP8266::CreateUDPConnection(uint8_t id, const String& ip, uint16_t localPort, uint16_t remotePort ) {
	if( id > 5) // ESP max id is 4
		return false;

	StrCmd cmd = "AT+CIPSTART=";
	cmd+= id;
	cmd += ",\"UDP\",\"";
	cmd += ip;
	cmd += "\",";
	cmd += remotePort; // May be any random value
	cmd += ",";
	cmd += localPort;
	cmd += ",0";

	return SendData(cmd, 200);
}

bool ESP8266::SendUDPPacket(uint8_t conID, uint8_t* packet, uint16_t size) {
	StrCmd cmd = "AT+CIPSEND=";
	cmd += conID;
	cmd += ",";
	cmd += size;

	SendData(cmd, 200);

	String resp;

	return SendData( packet, size, 0, &resp);
}




void ESP8266::SetBaud(uint32_t baud) {
	StrCmd cmd = "AT+CIOBAUD=";
	cmd += baud;

	SendData(cmd, 200);
	UART_Init( baud );
}
