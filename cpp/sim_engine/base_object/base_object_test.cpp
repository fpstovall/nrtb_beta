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

#include "base_object.h"
#include <iostream>
#include <string>

using namespace nrtb;
using namespace std;

struct gravity : public abs_effector
{
  triplet g = triplet(0,-9.81,0);

  virtual effector_p clone()
  {
    return effector_p(new gravity(*this));
  };
  
  gravity()
  {
    handle = "gravity";
  };
  
  std::string as_str()
  {
    std::stringstream returnme;
    returnme << handle << "_" << id << "=" << g;
    return returnme.str();
  };
  
  bool commandable() { return true; };
  
  bool command(std::string cmd, std::string & response)
  {
	  bool returnme = false;
	  if (cmd == "grav") 
	  {
		  response = "downwards";
		  returnme = true;
	  };
	  return returnme;
  };
  
  bool tick(base_object & o, float quanta)
  {
    o.accel_mod += g;
    return false;
  };
};

struct rocket : public abs_effector
{
  int time {0};
  
  virtual effector_p clone()
  {
    return effector_p(new rocket(*this));
  };

  triplet impulse = triplet(0.0,10000.0,0.0);
  int burn_time = 3;
  
  std::string as_str()
  {
    std::stringstream returnme;
    returnme << "rocket_" << id << "=" << impulse;
    return returnme.str();
  };

  bool commandable() { return true; };
  
  bool command(std::string cmd, std::string & response)
  {
	  bool returnme = false;
	  if (cmd == "rocket") 
	  {
		  response = "upwards";
		  returnme = true;
	  };
	  return returnme;
  };
  
  
  bool tick(base_object & o, float quanta)
  {
    if (time <= burn_time)
    {
      o.force += impulse;
      time++;
    }
    else
    {
      impulse = 0;
      o.drop_attrib(id);
    };
    return false;
  };  
};

struct torquer : public abs_effector
{
  rotatable torque;
  
  virtual effector_p clone()
  {
    return effector_p(new torquer(*this));
  };
  
  torquer()
  {
    handle = "torquer";
    torque.set(triplet(0,0,100));
  };
  
  string as_str()
  {
    stringstream returnme;
    returnme << handle << "_" << id << "=" << torque.angles();
    return returnme.str();
  };
  
  bool commandable() { return true; };
  
  bool command(std::string cmd, std::string & response)
  {
	  bool returnme = false;
	  if (cmd == "torq") 
	  {
		  response = "around";
		  returnme = true;
	  };
	  return returnme;
  };
  
  
  bool tick(base_object & o, float quanta)
  {
    o.torque.set(torque);
    return false;
  };
};

struct my_object : public base_object
{
  object_p clone() 
  {
    my_object * returnme = new my_object(*this);
    returnme->pre_attribs = get_pre_attribs_copy();
    returnme->post_attribs = get_post_attribs_copy();
    return object_p(returnme);
  };

  bool apply_collision(object_p o, float quanta) 
  {
    return false;
  };
};

int main()
{
  bool failed = false;
  cout << "========== base_object test ============="
    << endl;
  float quanta = 1.0/50.0;

//  cout << "Object setup:" << endl;
  my_object rocket_ball;
  rocket_ball.mass = 100;
  rocket_ball.bounding_sphere.center = triplet(0);
  rocket_ball.bounding_sphere.radius = 0.5;
//  cout << rocket_ball.as_str() << endl;
  
  rocket_ball.add_pre(effector_p(new gravity));
//  cout << rocket_ball.as_str() << endl;
  
  // test effector_p add.
  effector_p rp(new rocket);
  rocket_ball.add_pre(rp);
//  cout << rocket_ball.as_str() << endl;  
  
  // test command method
  std::string cmd, s;
  cmd = "rocket";
  if (rocket_ball.command(cmd,s))
  {
	  if(s != "upwards") failed = true;
  }
  else failed = true;
  cout << cmd << ":" << s << endl;
  
  cmd = "grav";
  if (rocket_ball.command(cmd,s))
  {
	  if(s != "downwards") failed = true;
  }
  else failed = true;
  cout << cmd << ":" << s << endl;
  
  cmd = "nop";
  if (rocket_ball.command(cmd,s))
  {
	  failed = true;
	  cout << cmd << ":" << s << endl;
  };
  
  cout << "command : " << (failed ? "FAILED" : "PASSED") << endl;
  
  cout << "Launch:" << endl;
  int time = 0;
  // initial burn.
  for (time; time<5; time++)
  {
//    cout << time*0.02 << " sec."<< endl;
    rocket_ball.tick(quanta);
    rocket_ball.apply(quanta);
//    cout << rocket_ball.as_str() << endl;
  };
  
  // coast to peak altitude
  while (rocket_ball.velocity.y > 0.0)
  {
    time++;
    rocket_ball.tick(quanta);
    rocket_ball.apply(quanta);    
  };
  cout << "Peak:" << time*0.02 << " sec."<< endl;
//  cout << rocket_ball.as_str() << endl;
  failed = failed or (time != 41);

  // Fall back down.
  while (rocket_ball.location.y > 0.0)
  {
    time++;
    rocket_ball.tick(quanta);
    rocket_ball.apply(quanta);    
  };
  cout << "Impact:" << time*0.02<< " sec." << endl;
//  cout << rocket_ball.as_str() << endl;
  failed = failed or (time != 80);
  
  cout << "** Flight Test: " << (failed ? "Failed" : "Passed") << endl;
  
  // ****** base_object spin test *******
  
  my_object spinner;
  spinner.mass = 100;
  spinner.bounding_sphere.radius = 1;
  spinner.add_pre(effector_p(new torquer));
  float q = 1.0/50.0;
  int tm = 0;
  for (;tm<50;tm++)
  {
    spinner.tick(q);
    spinner.apply(q);    
  };
  bool rf = false;
  float dv = fabs(spinner.rotation.angles().z - 2);
  if (dv > 1e-5)
  {
    rf = true;
    cout << "** base_object torque test: " 
      << dv << endl;
  };
  dv = fabs(spinner.attitude.angles().z - 1.0);
  if (dv > 1e-5)
  {
    rf = true;
    cout << "** base_object attitude test: " 
      << dv << endl;
  };
  cout << "** base_object spin test: " 
    << (rf ? "Failed" : "Passed") << endl;
  failed = failed or rf;
  
  // ********* collision tests ***********
  // -- fixed v. mobile test.
  my_object fixed = rocket_ball;
  fixed.location = triplet(1,0,0);
  fixed.velocity = triplet(-50,0,0);
  object_p mobile(rocket_ball.clone());
  mobile->velocity = 0;
  mobile->bounding_sphere = fixed.bounding_sphere;
  stringstream results;
  
  // check at various distances.
  mobile->location = triplet(0,2,0);
  mobile->velocity = triplet(0,-2,0);


  bool t = fixed.check_collision(mobile,1.0/50.0);
  results << fixed.location.range(mobile->location)
    << "=" << t << ",";
  
  mobile->location = triplet(0,1.01,0);
  t = fixed.check_collision(mobile,1.0/50.0);
  results << fixed.location.range(mobile->location)
    << "=" << t << ",";

  mobile->location = triplet(0,1,0);
  t = fixed.check_collision(mobile,1.0/50.0);
  results << fixed.location.range(mobile->location)
    << "=" << t << ",";

  mobile->location = triplet(0);
  t = fixed.check_collision(mobile,1.0/50.0);
  results << fixed.location.range(mobile->location)
    << "=" << t;
  
  bool c = results.str() != "2.23607=0,1.4213=1,1.41421=1,1=1";
  cout << results.str() << endl;
  cout << "** Collision Test: " << (c ? "Failed" : "Passed") << endl;
  
  // == quick clone test
  
  bool cl = mobile->as_str() != "ID=0:loc=(0,0,0):att=(0,0,0):vel=(0,-2,0):rot=(0,0,0):f=(0,0,0):t=(0,0,0):acc_mod=(0,-9.81,0):r_mod=(0,0,0):mass=100:mass_mod=0:b_sphere=(0,0,0),0.5:pre=gravity_0=(0,-9.81,0);:posts=";
  
  cout << "** clone() test: " << (cl ? "Failed" : "Passed") << endl;

  failed = failed or c or cl;
  
  //******* rotatable tests ********
  
  bool rfailed = false;
  rotatable r1;
  // add, set, get_cos, get_sin, angles tests.
  triplet forward(r1.get_cos().z, r1.get_sin().z,0);
  float eight = pi/4;
  for(int i=0; i<8; i++)
  {
    float p = eight*(i+1);
    r1.add(triplet(0,0,eight));
    triplet cart(r1.get_cos().z, r1.get_sin().z,0);
    if (fabs(cart.magnatude()-1.0) > 1e-5)
    {
      rfailed = true;
      cout << "** Cartisian conversion failed" << endl;
    };
  };
  // verify add accuracy.
  dv = r1.angles().z-r1.period;
  if (fabs(dv) > 1e-5)
  {
    rfailed = true;
    cout << "** rotation.add() failed " << dv << endl;
  };
  // rotatable scale test
  r1.scale(triplet(2,2,2));
  dv = r1.angles().magnatude() - (r1.period*2.0);
  if (fabs(dv) > 1e-5)
  {
    rfailed = true;
    cout << "** rotation.scale() failed " << dv << endl;
  };
  // rotatable trim test.
  r1.trim();
  dv = r1.angles().magnatude();
  if (dv > 1e-5)
  {
    rfailed = true;
    cout << "** rotation.trim() failed " << dv << endl;
  };
  // rotatable.apply_force test.
  r1.set(triplet(0,0,0));
  r1.apply_force(100,1,triplet(100,100,100),1);
  triplet dvt = r1.angles();
  if ((dvt.x != dvt.y) or (dvt.y != dvt.z) or (dvt.x != 2.0))
  {
    rfailed = true;
    cout << "** rotatable.apply_force() failed " << dvt << endl;
  };
  
  // sum it up.
  cout << "** rotatable test: " 
    << (rfailed ? "Failed" : "Passed") << endl;
  
  failed = failed or rfailed;
    
  cout << "=========== base_object test complete ============="
    << endl;
  
  return failed;
};



























