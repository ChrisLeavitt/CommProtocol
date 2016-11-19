/*
  PacketTable, handles an ADT storage for associating Packets.

  Copyright (C) 2016  Michael Wallace, Kartik Soni, Mario Garcia.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (At your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <CommProto/pkg/packettable.h>
#include <CommProto/architecture/macros.h>
#include <CommProto/abstractpacket.h>
#include <CommProto/callback.h>


#if (COM_TARGET_OS != COM_OS_WINDOWS) && (COM_TARGET_OS != COM_OS_APPLE)
 #include <tr1/functional>
 #define COMMSTD std::tr1
#else 
 #include <functional>
 #define COMMSTD std
#endif


#define DEFAULT_TABLE_SIZE 1024


namespace comnet {
namespace pkg {


PacketTable::PacketTable(uint32_t setSize)
: table(new Pair*[setSize])
, numOfPairs(0)
, tableSize(setSize)
{

  nullifyAttributesInTable(table, tableSize);

}


PacketTable::PacketTable() 
: table(new Pair*[DEFAULT_TABLE_SIZE])
, numOfPairs(0)
, tableSize(DEFAULT_TABLE_SIZE)
{

  nullifyAttributesInTable(table, tableSize);

}


PacketTable::~PacketTable()
{
  Pair* pair;
  for (int32_t i = 0; i < tableSize; ++i) {
    pair = *(table+i);
    
    if (pair) {
      if (pair->packet) {
	delete pair->packet;
	pair->packet = NULL;
      }

      if (pair->callback) {
	delete pair->callback;
	pair->callback = NULL;
      }

      delete pair;
      pair = NULL;
    }
  }

  delete[] table;
}


bool PacketTable::Insert(const AbstractPacket* key, const Callback* callback) {
  if (!key) {
    return false;
  }

  bool stored = false;
  bool willStore = true;
  uint32_t hash = KeyHash(key->GetId());
  Pair* pair = allocate_pointer(Pair);

  pair->packet = (AbstractPacket* )key;
  pair->callback = (Callback* )callback;

  int saved = hash;
  while ( (*(table+hash)) != NULL  && (*(table+hash))->packet->GetId() != key->GetId() ) {
    hash = TraverseIndex(hash);
    
    if (hash == saved) {
      willStore = false;
      break;
    }
  }


  if ( *(table+hash) == NULL ) {
    *(table+hash) = pair;
    numOfPairs++;
    stored = true;
  } else if (willStore) {
    free_pointer(pair->packet);
    free_pointer(pair);
    free_pointer((*(table+hash))->callback);
    (*(table+hash))->callback = (Callback* )callback;
    stored = true;
  }
  
  return stored;
}


Callback* PacketTable::GetCallback(uint32_t key) {
  uint32_t hash = KeyHash(key);
  Callback* result = NULL;

  if ( *(table+hash) != NULL ) {
    result = (*(table+hash))->callback;
  }
  
  return result;
}


AbstractPacket* PacketTable::GetPacket(uint32_t key) {
 uint32_t hash = KeyHash(key);
  AbstractPacket* result = NULL;

  int32_t saved = hash;
  
  while ( *(table+hash) != NULL  && (*(table+hash))->packet->GetId() != key ) {
    hash = TraverseIndex(hash);
    
    if (hash == saved) {
      break;
    }
  }

  if ( *(table+hash) != NULL) {
    result = (*(table+hash))->packet;
  }
  
  return result;
}


bool PacketTable::Remove(uint32_t key) {
  return false;
}


bool PacketTable::Resize(uint32_t newSize) {
  return false;
}


int32_t PacketTable::TraverseIndex(int32_t i) {
  i++;
  if (i >= tableSize) {
    i = 0;
  }

  return i;
}


uint32_t PacketTable::KeyHash(uint32_t key) {
  return (COMMSTD::hash<uint32_t>()(key) % tableSize);
}
} // namespace Pkg
} // namespace Comnet
