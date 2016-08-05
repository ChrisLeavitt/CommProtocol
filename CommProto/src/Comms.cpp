#include <CommProto/Comms.h>

#include <CommProto/network/UDP.h>
#include <CommProto/network/Serial.h>

using namespace Comnet;

/***********************************************/
/******************* Private *******************/
/***********************************************/


/**
Helper function to convert between C++ and C function signatures
due to casting as a class member being incompatible with C style
thread creation APIs. Static linkage helps with that.
*/
void* Comms::commuincationHelperSend(void* context)
{
	return ((Comms*)context)->commuincationHandlerSend();
}

void* Comms::commuincationHelperRecv(void* context)
{
	return ((Comms*)context)->commuincationHandlerRecv();
}


/** function for communication thread */
void* Comms::commuincationHandlerSend()
{
	while (isRunning)
	{
		if (!sendQueue->isEmpty())
		{
			//send data here
			ObjectStream *temp = sendQueue->front();
			sendQueue->deQueue();
			connectionLayer->send(temp->headerPacket.destID, temp->getBuffer(), temp->getSize());
			delete temp;
			temp = NULL;
		}
	}
	return 0;
}

/** function for communication thread */
void* Comms::commuincationHandlerRecv()
{
	while (isRunning)
	{
		//send data here
		uint8_t streamBuffer[MAX_PACKET_SIZE];
		uint32_t recvLen = 0;
		connectionLayer->recv(streamBuffer, &recvLen);
		ObjectStream *temp = new ObjectStream();
		temp->setBuffer((char*)streamBuffer, recvLen);

		temp->deserializeHeder();

		//
		// decrtyp object stream here with call
		//

		//create abstract data
		//figure out packet type
		//unpack abstract data

		//
		// call linker here
		//

		//
		// decide if orphan figure out about delteing temp if not orphan
		//
		


		recvQueue->enQueue(temp);						
	}
	return 0;
}

/***********************************************/
/******************* Public  *******************/
/***********************************************/
Comms::Comms(uint8_t platformID)
{
	recvQueue = new Comnet::Tools::DataStructures::AutoQueue <Serialization::ObjectStream*>;
	sendQueue = new Comnet::Tools::DataStructures::AutoQueue <Serialization::ObjectStream*>;
	isRunning = false;
	isPaused = false;
	mutex_init(&sendMutex);
	mutex_init(&recvMutex);
	connectionLayer = NULL;
	setID(platformID);
	
}

Comms::~Comms()
{
	isRunning = false;
	if(connectionLayer != NULL)
	{
		delete connectionLayer;
	}
	mutex_destroy(&sendMutex);
	mutex_destroy(&recvMutex);

	delete recvQueue;
	delete sendQueue;
}

bool Comms::initConnection(CommsLink_type_t connectionType, const char* port, const char* address, uint32_t baudrate)
{
	uint16_t length = 0;
	switch (connectionType)
	{
		case UDP_LINK:
		{			
			
			str_length(address, length);
			if (length < ADDRESS_LENGTH)
			{							
				connectionLayer = new UDP();
				return connectionLayer->initConnection(port, address);
			}
			break;
		}
		case SERIAL_LINK:
		{
			
			str_length(address, length);
			if (length < ADDRESS_LENGTH)
			{
				connectionLayer = new Serial();
				return connectionLayer->initConnection(port, NULL, baudrate);
			}
			break;
		
		}
		case ZIGBEE_LINK:
		{}
		default:
		{return false;}
	}
	return true;
}

bool Comms::addAddress(uint8_t destID, const char* address , uint16_t port)
{
	if (connectionLayer == NULL)return false;
	return connectionLayer->addAddress(destID, address, port);
}

bool Comms::removeAddress(uint8_t destID)
{
	if (connectionLayer == NULL)return false;
	return connectionLayer->removeAddress(destID);
}

bool Comms::send(AbstractPacket* packet, uint8_t destID, uint16_t messageID)
{
	if (connectionLayer == NULL) return false;
	{
		ObjectStream *temp = new ObjectStream();		
		//packet->unpack(*temp);		
		header_t header;
		header.destID = destID;
		header.sourceID = this->getID();
		header.messageID = messageID;
		header.messageLength = temp->getSize();
		//
		//call encryption here
		//
		temp->serializeHeader(header);
		sendQueue->enQueue(temp);
	}

	return true;
}

AbstractPacket* Comms::receive(uint8_t&  sourceID, uint16_t& messageID)
{
	if (connectionLayer == NULL) return false;
	{
		if (!recvQueue->isEmpty())
		{
			cout << "Message recv in Comms" << endl;
			recvQueue->deQueue();
		}
	}
	return NULL;
}

int32_t Comms::run()
{
	thread_create(&this->communicationThreadSend, commuincationHelperSend, this);
	thread_create(&this->communicationThreadRecv, commuincationHelperRecv, this);
	isRunning = true;

	return 1;
}


int32_t Comms::stop()
{
	isRunning = false;
	return 1;
}



