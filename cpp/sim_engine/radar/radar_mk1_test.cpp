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

#include "radar_mk1.h"
#include <base_object.h>
#include <iostream>
#include <string>

using namespace nrtb;
using namespace std;

struct my_object : public base_object
{
  my_object() : radar(radar_mk1(*this)) {};
  
  radar_mk1 radar;
  
  bool apply_collision(object_p o) 
  {
    return true;
  };
  
  base_object * clone()
  {
    my_object * returnme = new my_object(*this);
    returnme->pre_attribs = get_pre_attribs_copy();
    returnme->post_attribs = get_post_attribs_copy();
    return returnme;
  };
};

int main()
{
  bool failed = false;
  cout << "========== radar_mk1 test ============="
    << endl;
 
  // get a couple target objects.
  my_object * o1 = new my_object;
  my_object * o2 = new my_object;
  o2->location = triplet(0,1,1);
    
  // start a sim_core;
  sim_core & w = global_sim_core::get_reference();
  w.start_sim();
  
  // Insert objects in the sim.
  w.add_object(object_p(o1));
  w.add_object(object_p(o2));
  chrono::milliseconds pause(50);
  this_thread::sleep_for(pause);
  
  // Verify they see each other properly.
  string o1c = o1->radar.contacts();
  string o2c = o2->radar.contacts();  
  
  w.stop_sim();

  cout << "from o1: " << o1c << endl;
  cout << "from o2: " << o2c << endl;
  
  cout << "=========== radar_mk1 test complete ============="
    << endl;
  
  return failed;
};



























