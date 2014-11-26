/***********************************************
 This file is part of the NRTB project (https://github.com/fpstovall/nrtb_alpha).
 
 NRTB is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 NRTB is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with NRTB.  If not, see <htttp://www.gnu.org/licenses/>.
 
 **********************************************/
 
#ifndef bot_interfaces_header
#define bot_interfaces_header

#include <string>

namespace nrtb
{

/// allows a module to send to the bot's BCP  
class bcp_sender
{
  virtual void send_to_bcp(std::string msg) = 0;
};

/// allows a module to insert a command into the
/// bot's BCP command stream.
class cmd_interface
{
  virtual std::string bot_cmd(std::string cmd) = 0;
};

/// Allows a module to sleep until the next tick.
class tickable
{
  virtual void wait_for_tick() = 0;
};

} // namepace nrtb

#endif // bot_interfaces_header
