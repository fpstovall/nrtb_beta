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

#include "diff_steer.h"
#include <gravity.h>
#include <iostream>
#include <string>

using namespace nrtb;
using namespace std;

/*******************************
 * WARNING:
 * Don't do this at home kids. This method of recording
 * process information would be inheriently dangerious 
 * in the sim_core context. We can only use it here 
 * because we are ticking the object directly, something
 * which never happens in sim_engine context.
 * ******************************/
 
vector<triplet> locations;
vector<triplet> velocities;

// this is strictly designed as a post_attrib.
struct recorder : abs_effector
{
  string as_str() { return "recorder"; };
  abs_effector * clone()
  {
    return new recorder(*this);
  };
  bool tick(base_object & o, int time)
  {
    locations.push_back(o.location);
    velocities.push_back(o.velocity);
  };
};

struct driver : public base_object
{
  // diff_steer interface
  shared_ptr<diff_steer>  mobility;
  // control interface
  void drive(float p)
  {
    mobility->drive(p);
  };
  void brake(float b)
  {
    mobility->brake(b);
  };
  void turn(float t)
  {
    mobility->turn(t);
  };
  void lockdown()
  {
    mobility->lockdown();
  };
  // constructor
  driver()
  {
    mobility.reset(new diff_steer(*this,100,1000,100,10,8));
    add_pre(new norm_gravity);
    add_post(new recorder);
    mass=100.0;
    bounding_sphere.radius = 1.0;
  };
  // clone method
  base_object * clone()
  {
    driver * t = new driver(*this);
    t->pre_attribs = get_pre_attribs_copy();
    t->post_attribs = get_post_attribs_copy();
    return t;
  };
  // nop collision application.
  bool apply_collision(object_p o) {return false;};
};

typedef map<std::string,double> rtype;
rtype report(vector<triplet> v)
{
  double min = 1e6;
  double max = 0.0;
  double avg = 0.0;
  double count = 0;
  for(auto t: v)
  {
    count++;
    min = (min < t.z) ? min : t.z;
    max = (max > t.z) ? max : t.z;
    avg += t.z;
  };
  avg /= count;
  rtype returnme;
  returnme["min"] = min;
  returnme["max"] = max;
  returnme["avg"] = avg;
  return returnme;
};

string write_details(rtype l, rtype v)
{
  stringstream s;
  s << "Alt : " << l["min"] << ":" 
    << l["avg"] << ":" << l["max"] 
    << "\nVel : " << v["min"] << ":" 
    << v["avg"] << ":" << v["max"];
  return s.str();
};

int main()
{
  bool failed = false;
  cout << "========== diff_steer test ============="
    << endl;

  try
  {
    driver test_ob;
    test_ob.lockdown();
    for(int i=0; i<2; i++)
    {
  //    cout << test_ob.as_str() << endl << endl;
      test_ob.tick(i);
  //    cout << test_ob.as_str() << endl << endl;
      test_ob.apply(i,1/50.0);
  //    cout << test_ob.as_str() << endl << endl;
      cout << "--------------------"  << endl << endl;
    };
    cout << write_details(report(locations),report(velocities))
      << endl;
  }
  catch (base_exception &e)
  {
    failed = true;
    cout << "Exception: " << e.comment() << endl;
  }
  catch (...)
  {
    failed = true;
    cout << "Unspecified Exception!" << endl;
  };
  cout << "=========== diff_steer test complete ============="
    << endl;
  
  return failed;
};



























