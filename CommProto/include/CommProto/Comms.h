#ifndef COMMS_H
#define COMMS_H

#include <stdint.h>//needed for bit size variables
#include <CommProto/tools/data_structures/AutoQueue.h>//marios wrapper of queue
#include <CommProto/tools/data_structures/interface/InterfaceQueue.h>
#include <CommProto/network/CommsLink.h> //communication layer interface/abstract base class
#include <CommProto/network/CommSocket.h> // 
#include <CommProto/HeaderPacket.h>//Header which declares packet structs
#include <CommProto/architecture/os/include_defines.h>
#include <CommProto/architecture/os/os_threads.h>//method to create threads
#include <CommProto/architecture/os/os_mutex.h>
#include <CommProto/architecture/macros.h>//str_lgnth(char*, int)
#include <CommProto/serialization/ObjectStream.h>
#include <CommProto/AbstractPacket.h>
#include <CommProto/CommNode.h>



#include <iostream>//testing only
using namespace std;//testing only

namespace Comnet
{
	typedef enum
	{
		UDP_LINK,
		SERIAL_LINK,
		ZIGBEE_LINK
	}CommsLink_type_t;

	class Comms: public CommNode {
	private:
		mutex_t sendMutex;
		mutex_t recvMutex;

		/** Boolean if communication thread running*/
		bool isRunning;
		/** Check if the program is paused*/
		bool isPaused;
		/** Encryption key*/
		uint8_t key[KEY_LENGTH];
		/** Method to read key form text file*/
		void loadKey();

		/** Length of data buffer for communication stream */
		uint32_t rx_length;

		/** Queues for application layer to push messages or pop messages */

		Comnet::Tools::DataStructures::Interface::Queue <Serialization::ObjectStream*> *recvQueue;
		Comnet::Tools::DataStructures::Interface::Queue <Serialization::ObjectStream*> *sendQueue;

		/** Thread to run communication data */
		thread_t communicationThreadSend;
		thread_t communicationThreadRecv;

		/** Method to run in communication thread */
		void* commuincationHandlerSend();
		void* commuincationHandlerRecv();

		/**
		Helper function to convert between C++ and C function signatures
		due to casting as a class member being incompatible with C style
		thread creation APIs. Static linkage helps with that.
		*/
		static void* commuincationHelperSend(void* context);
		static void* commuincationHelperRecv(void* context);

		/** Polymorphic (Base Class) Communication link for connection code*/
		CommsLink *connectionLayer;


	public:		
		/** Constructor */
		Comms(uint8_t platformID);
		/** Destructor */
		~Comms();

		/** Method will init the connection device and return false if connection failed to init properly
		enum connection type to use serial, UDP, zigbee, ect
		port number is the comport or UDP port used will differ from windows and Unix for comports COM05 or /dev/ttyUSB05
		baud-rate is not used for for UDP which is not needed but for serial and zigbee it is needed for baud rate */
		bool initConnection(CommsLink_type_t connectionType, const char* port, const char* address = NULL, uint32_t baudrate = 0);

		/** The address entered will be paired for communication by destination ID
		Adding address can be a UDP IPV4 or hex MAC address for zigbee
		Adding an address is not need for serial and will default to ""*/
		bool addAddress(uint8_t destID, const char* address = NULL, uint16_t port = 0);

		/** Removing an address removes the known association of address and destination ID by using id*/
		bool removeAddress(uint8_t destID);

		bool send(AbstractPacket* packet, uint8_t destID, uint16_t messageID);
		AbstractPacket* receive(uint8_t&  sourceID, uint16_t& messageID);

		/** Method to start communication*/
		int32_t run();
		/** Method to toggle pause communication*/
		int32_t pause(){ return 0; }
		/** Method to stop communication*/
		int32_t stop();

	protected:
		// Nothing yet.
		void logToConsoles()
		{ }
	};//end Comms class      
}
#endif//end if COMMS_H
