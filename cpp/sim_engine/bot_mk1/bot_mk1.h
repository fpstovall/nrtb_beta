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
 along with NRTB.  If not, see <http://www.gnu.org/licenses/>.
 
 **********************************************/
 
#ifndef bot_mk1_header
#define bot_mk1_header

#include <map>
#include <bot_interfaces.h>
#include <base_socket.h>
#include <confreader.h>
#include <common_log.h>
#include <diff_steer.h>
#include <radar_mk1.h>
#include <hover.h>
#include <gravity.h>
#include <linear_queue.h>

namespace nrtb
{

/****************************************************
 * bot_mk1 is the prototype bot in the alpha system. 
 * As such it is subject to rapid change and may not 
 * be the prettiest code. In particular, much of the
 * following should probably be protected or private,
 * but we're leaving it public for now to simplify 
 * testing and experimentation.
 ***************************************************/  
struct bot_mk1: public abs_bot
{
  /* Constucts a bot at the supplied location and 
   *  and establishes the link to the BCP socket.
   *  link - a tcp_socket_p connected to the BCP.
   *  where - the locaiton the bot is to created at.
   */
  bot_mk1(tcp_socket_p link, triplet where);
  virtual ~bot_mk1();
  // polymophic copier
  virtual object_p clone();
  // returns true if the bot is functional.
  virtual bool IsAlive() { return ImAlive; };
  // sim methods
  // returns true if a collision is detected.
  virtual bool check_collision(object_p o, float duration);
  // the following return true if the object is destroyed.
  virtual bool tick(float duration);
  virtual bool apply(float duration);
  virtual bool apply_collision(object_p o, float duration);
  // bot_interfaces stuff;
  void send_to_bcp(std::string msg);
  void bot_cmd(std::string cmd);
  // simcore lock and unlock.
  void lock();
  void unlock();
private:
  // flags for auto commands (report every game turn)
  bool autor {false}; // auto radar contact report
  bool autol {false}; // auto lvar 
  /* Constucts a bot at the supplied location but 
   * without much of the setup completed. 
   */
  bot_mk1(triplet where);
  // additional data
  std::string name;  // autogenerated human readable ID
  std::atomic<bool> ImAlive;  // used to trigger shutdown of threads.
  // the robot's drive module.
  std::unique_ptr<diff_steer> drive;
  // connection to the controlling bot control program.
  tcp_socket_p BCP;
  // outbound message queue.
  linear_queue<std::string> to_BCP;
  // used to block threads during tick and apply.
  std::mutex cooking_lock;
  // handle for the reciever thread
  std::thread r_thread;
  // handle for the transmitter thread
  std::thread t_thread;
  // BCP communications managers;
  void receiver(); // -- thread which processes inbound BCP messages.
  void transmitter(); // -- thread which processes outbound BCP messages.
  void msg_router(std::string s); // -- Command processor.
};

} // namepace nrtb

#endif // bot_mk1_header
