/*
  Queue Interface for CommProtocol. 
  
  Copyright (C) 2016  Mario Garcia.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __INTERFACE_QUEUE_H
#define __INTERFACE_QUEUE_H

#include <CommProto/tools/data_structures/interface/Generic.h>

using namespace Comnet::Tools::DataStructures::Interface;

namespace Comnet {
namespace Tools {
namespace DataStructures {
namespace Interface {

/**
   QueueType holds the value of what ever the generic queue may be.
*/
_COMNET_PRIVATE_API_
enum QueueType {
  SINGLE_LINKED_QUEUE,
  DOUBLE_LINKED_QUEUE,
  PRIORITY_QUEUE,
  AUTO_QUEUE,
};
  
/** 
    Queue interface that must be inherited by all queue data structures. 
*/
_COMNET_PRIVATE_API_
template<typename _Ty>
class Queue : public Generic {
public:
  /**
     Queue Constructor.
  */
  Queue() { type = QUEUE; }
  /**
     Polymorphc Queue destructor.
  */
  virtual ~Queue()  { } 
  /**
     Get the size of this queue.
  */
  int32_t getSize() { return size; }
  /**
     get the data structure type.
  */
  DataStructureType getDataStructType() { return type; }
  /**
     Get the queue type.
  */
  virtual QueueType getQueueType() { return queueType; }
  /**
     Add a value into the queue.
  */
  virtual void enQueue(const _Ty& data) = 0;
  /**
     Remove a value from the queue.
  */
  virtual void deQueue() = 0;
  /**
     Check the front of this queue.
  */
  virtual const _Ty& front() = 0;
  /**
     Check the back of this queue.
  */
  virtual const _Ty& back() = 0;
protected:
  /**
     Holds the queue type value.
  */
  QueueType queueType;
};
} // Interface
} // DataStructures
} // Tools
} // Comnet
#endif // __INTERFACE_QUEUE_H