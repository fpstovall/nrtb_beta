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

#include <base_object.h>

namespace nrtb
{

/************************************************
 * bcp_sender is a functor interface for bots 
 * which simplifies returning messages to the 
 * bot control program. 
 ***********************************************/
class bcp_sender
{
public:
  virtual void send_to_bcp(std::string msg) = 0;
};

/************************************************
 * cmd_interface is a interface for bots
 * which allows the insertion of commands into
 * into the BCP command interface as if it was 
 * recieved from the BCP itself.
 ***********************************************/
class cmd_interface
{
public:
  virtual void bot_cmd(std::string cmd) = 0;
};

/*=======================================================
 * The following two intefaces provide a mechanism for 
 * a bot to implement internal functions called on a 
 * regular basis. Typically, the bot would arrange to 
 * call them once per game turn.
 *=====================================================*/

/******************************************************
 * tickable is a functor interface provided by modules 
 * wanting to plug into a bot's ticker mechanism to be 
 * called regularly. 
 *****************************************************/
class tickable
{
public:
  // method to be called by the bot's ticker
  virtual void operator()(float duration) = 0;
};

/******************************************************
 * ticker provides the methods and data required to 
 * allow a bot implementation to manage callbacks to 
 * modules which need to do some work each game cycle.
 *****************************************************/
class ticker
{
public:
  // stores modules implementing the tickable interface
  std::map<unsigned long long,tickable &> tickees;
  // Register a tickable to be called.
  void register_ticker(tickable & t);
  // Remove the tickable from the callback list.
  void deregister_ticker(tickable & t);
  // Call all tickables iteratively.
  void tick_all(float duration);
};

/****************************************************
 * abs_bot provides the common interfaces a bot needs
 * to be useful. In addition to agragating the 
 * interfaces, base_object::tick() is overridden to
 * to call ticker::tick_all() before calling the
 * original method and returning the results.
 ***************************************************/
struct abs_bot
: public base_object,
  public bcp_sender,
  public cmd_interface,
  public ticker
{
  bool tick(float duration);
};

} // namepace nrtb

#endif // bot_interfaces_header
