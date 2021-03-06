/***********************************************
 This file is part of the NRTB project (https://github.com/fpstovall/nrtb_beta).
 
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

#include "bot_mk1.h"
#include <iostream>
#include <string>
#include <future>

using namespace nrtb;
using namespace std;

shared_ptr<bot_mk1> test_bot;

int request_processor(tcp_server_socket_factory & server)
{
  int hits {0};
  bool done {false};
  while (!done)
  {
    try
    {
      auto sock = server.get_sock();
      // create test_bot
      test_bot.reset(new bot_mk1(std::move(sock),triplet(0.0)));
      cout << "New bot_mk1 created." << endl;
      // Update our hit count.
      hits++;
    }
    catch (tcp_server_socket_factory::queue_not_ready)
    {
      done = true;
    }
    catch (exception & e)
    {
      cout << "ReqProc recieved " 
        << typeid(e).name()
        << endl;
      done = true;
    };
  }; 
  cout << "Request processor shut down." << endl;
  return hits;
};


int main()
{
  bool failed = false;
  cout << "========== bot_mk1 test ============="
    << endl;

  // -- set up and start the listener
  string address = "localhost:64500";
  tcp_server_socket_factory test_server(address);
  test_server.start_listen();
  // start request processor
  auto rp_out = 
    async(launch::async,request_processor,std::ref(test_server));

  // -- connect (should create a bot_mk1).
  tcp_socket BCP;
  try 
  {
    BCP.connect(address);

    // first check.. did the bot say "READY"?
    string msg = gsub(BCP.getln("\r",64,2),"\r","");
    bool bad = (msg != "READY") or !(test_bot->IsAlive());
    failed = failed or bad;
    cout << "READY: " << ( bad ? "FAILED" : "Passed" ) 
      << " '" << msg << "'" << endl;
      
    // get position and velocity data.
    BCP.put("bot lvar\r");
    msg = gsub(BCP.getln("\r",64,2),"\r","");
    bad = (msg != "bot lvar (0,0,2.5) (0,0,0) (0,0,0) (0,0,0)") 
      or !(test_bot->IsAlive());
    failed = failed or bad;
    cout << "bot lvar: " << ( bad ? "FAILED" : "Passed" ) 
      << " '" << msg << "'" << endl;
    
    // get health data.
    BCP.put("bot health\r");
    msg = gsub(BCP.getln("\r",64,2),"\r","");
    bad = (msg != "bot health 100") or !(test_bot->IsAlive());
    failed = failed or bad;
    cout << "bot health: " << ( bad ? "FAILED" : "Passed" ) 
      << " '" << msg << "'" << endl;
    
    // get bot error response.
    BCP.put("bot error\r");
    msg = gsub(BCP.getln("\r",64,2),"\r","");
    bad = (msg != "bad_cmd \"bot error\"") or !(test_bot->IsAlive());
    failed = failed or bad;
    cout << "bot cmd error: " << ( bad ? "FAILED" : "Passed" ) 
      << " '" << msg << "'" << endl;
    
    // get drive settings.
    BCP.put("drive status\r");
    msg = gsub(BCP.getln("\r",64,2),"\r","");
    bad = (msg != "drive status 0 1 0") or !(test_bot->IsAlive());
    failed = failed or bad;
    cout << "drive response: " << ( bad ? "FAILED" : "Passed" ) 
      << " '" << msg << "'" << endl;

    // Verify radar command/response
    BCP.put("radar status\r");
    msg = gsub(BCP.getln("\r",64,2),"\r","");
    bad = (msg != "radar status 1") or !(test_bot->IsAlive());
    failed = failed or bad;
    cout << "radar response: " << ( bad ? "FAILED" : "Passed" ) 
      << " '" << msg << "'" << endl;
          
    // verify running statuses.
    float d = 1.0/50.0;
    bad = test_bot->tick(d) or test_bot->apply(d);
    failed = failed or bad;
    cout << "tick and apply: " << ( bad ? "FAILED" : "Passed" ) 
      << endl;
    
    // clone test.
    object_p cobj = test_bot->clone();
    bad = cobj->id != test_bot->id;
    cout << "clone(): " << ( bad ? "FAILED" : "Passed" )
      << endl;
    failed = failed or bad;
    
    // verify bot shutdown on connection close
    BCP.close();
    chrono::milliseconds pause(20);
    this_thread::sleep_for(pause);
    bad = test_bot->IsAlive();
    failed = failed or bad;
    cout << "shutdown on close: " << ( bad ? "FAILED" : "Passed" )
      << endl;
      
    // verify stopped statuses.
    bad = !test_bot->tick(d) or !test_bot->apply(d);
    failed = failed or bad;
    cout << "tick and apply: " << ( bad ? "FAILED" : "Passed" ) 
      << endl;    
      
  }
  catch (base_exception & e)
  {
    cout << "EXCEPTION: " << e.what() << "\n\t"
      << e.comment() << endl;
  }
  catch (exception & e)
  {
    cout << "EXCEPTION: " << e.what() << endl;
  }
  catch (...)
  {
    cout << "UNKNOWN EXCEPTION" << endl;
  };
  //  sender.put(sendme);    

  try { BCP.close(); } catch (...) {};
  try { test_server.stop_listen(); } catch (...) {};
    
  cout << "=========== bot_mk1 test complete ============="
    << endl;
  
  return failed;
};



























