/*
  Runs a thread to update pingers.

  Copyright (C) 2016  Alex Craig, Michael Wallace, Mario Garcia.

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

#include <CommProto/ping/ping_manager.h>
#include <CommProto/ping/sync_manager.h>
#include <CommProto/comms.h>

namespace comnet {
namespace ping {
error_t PingCallback(const comnet::Header & header, PingPacket & packet, comnet::Comms & node)
{
  if (packet.IsPing())
  {
    if (node.GetPingManager()->CanPong(header.source_id))
    {
      PingPacket pingPacket;
      pingPacket.SetPing(false);
      node.Send(pingPacket, header.source_id);
    }
  }
  return CALLBACK_SUCCESS | CALLBACK_DESTROY_PACKET;
}

PingManager::PingManager(Comms* ownerComms)
  :std::enable_shared_from_this <PingManager>(), ownerComms(ownerComms), pingSendThread(nullptr)
{
    pingPacket = new PingPacket();
				syncManager = std::make_shared <SyncManager>(ownerComms);
}

void PingManager::LinkCallbacks()
{
  ownerComms->LinkCallback(pingPacket, new Callback((comnet::callback_t)PingCallback));
		syncManager->LinkCallbacks();
}

bool PingManager::Run()
{
		running = true;
		pingSendThread = new CommThread(&PingManager::HandlePingUpdate, shared_from_this());
		pingSendThread->Detach();
		syncManager->Run();
		return true;
}

void PingManager::AddPinger(uint8_t destID)
{
		activePingersMutex.Lock();
		activePingers.emplace_back(destID);
		destPingerMapMutex.Lock();
		destPingerMap.emplace(std::make_pair(destID, --activePingers.end()));
		destPingerMapMutex.Unlock();
		syncManager->AddUnsyncedPinger(&activePingers.back());
		activePingersMutex.Unlock();
}

void PingManager::RemovePinger(uint8_t destID)
{
		CommLock lock(destPingerMapMutex);
		auto mapIter = destPingerMap.find(destID);
		if (mapIter != destPingerMap.end())
		{
				if (!mapIter->second->IsSynced())
				{
						syncManager->RemoveUnsyncedPinger(&(*mapIter->second));
				}
				if (mapIter->second->IsInactive())
				{
						inactivePingersMutex.Lock();
						inactivePingers.erase(mapIter->second);
						inactivePingersMutex.Unlock();
				}
				else
				{
						activePingersMutex.Lock();
						activePingers.erase(mapIter->second);
						activePingersMutex.Unlock();
				}
				destPingerMap.erase(mapIter);
		}
}

void PingManager::SendPingPacket(uint8_t destID)
{
  PingPacket sendPacket;
  sendPacket.SetPing(true);
  ownerComms->Send(sendPacket, destID);
}

void PingManager::Stop()
{
  runningMutex.Lock();
  running = false;
  runningMutex.Unlock();
		syncManager->Stop();
}

void PingManager::SyncTime(uint8_t nodeID, int32_t timeOff)
{
		CommLock lock(destPingerMapMutex);
		auto mapIter = destPingerMap.find(nodeID);
		if (mapIter != destPingerMap.end() && !mapIter->second->IsSynced())
		{
				syncManager->SyncTime(&(*mapIter->second), timeOff);
		}
}

int16_t PingManager::GetPing(uint8_t nodeID)
{
		CommLock lock(destPingerMapMutex);
		auto mapIter = destPingerMap.find(nodeID);
		if (mapIter != destPingerMap.end())
		{
				return mapIter->second->GetPing();
		}
		return -1;
}

PingManager::~PingManager()
{
		free_pointer(pingPacket);
		free_pointer(pingSendThread);
}

void PingManager::TransferToActivePingers(std::list<Pinger>::iterator it)
{
		if (!it->IsSynced())
		{
				syncManager->AddUnsyncedPinger(&(*it));
		}
  activePingers.splice(activePingers.end(), inactivePingers, it);
  std::string debugMsg = "Pinger with destID ";
  debugMsg += std::to_string((int)it->GetDestID());
  debugMsg += " in NodeID ";
  debugMsg += std::to_string((int)ownerComms->GetNodeId());
  debugMsg += " was set to active";
  comnet::debug::Log::Message(comnet::debug::LOG_DEBUG, debugMsg);
}

void PingManager::TransferToInactivePingers(std::list<Pinger>::iterator it)
{
		if (!it->IsSynced())
		{
				syncManager->RemoveUnsyncedPinger(&(*it));
		}
  inactivePingers.splice(inactivePingers.end(), activePingers, it);
  std::string debugMsg = "Pinger with destID ";
  debugMsg += std::to_string((int)it->GetDestID());
  debugMsg += " in NodeID ";
  debugMsg += std::to_string((int)ownerComms->GetNodeId());
  debugMsg += " was set to inactive";
  comnet::debug::Log::Message(comnet::debug::LOG_DEBUG, debugMsg);
}

} //namespace ping
} //namespace comnet