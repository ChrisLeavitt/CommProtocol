#ifndef __XBEE_LINK_H
#define __XBEE_LINK_H

#include <CommProto/network/CommsLink.h>
#include <CommProto/tools/data_structures/interface/InterfaceList.h>
#include <CommProto/architecture/macros.h>

namespace Comnet {
namespace Network {
namespace Experimental {


using namespace Comnet::Tools::DataStructures::Interface;

class XBee;


class XBeeLink : public CommsLink {
public:
  XBeeLink()
    { }

  ~XBeeLink() {
    for (int i = 0; i < xbees->getSize(); ++i ) {
      XBee* xbee = xbees->at(i);
      free_pointer(xbee);
    }
  
    free_pointer(xbees);
  }
  /**
     Initialize a connection for this node.
   */
  bool initConnection(const char* port = NULL, const char* = NULL, uint32_t baudrate = 0);
  /**
     Adds an address to the link.
   */
  bool addAddress(uint8_t destId, const char* address = NULL, int16_t port = 0);
  /**
     Remove an address from the link.
   */
  bool removeAddress(uint8_t destId);
  /**
     Send a message to the specified id of which an address is associated with.
   */
  bool send(uint8_t destId, uint8_t* txData, int32_t txLength);
  /**
     Receive a message from a protocol id.
   */
  bool recv(uint8_t* rxData, uint32_t* rxLength);

private:

  List< XBee* >* xbees;
};
} // namespace Experimental
} // namespace Network
} // namespace Comnet
#endif // __XBEE_HANDLER_H