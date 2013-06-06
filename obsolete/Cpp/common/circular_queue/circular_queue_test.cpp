/***********************************************
 This file is part of the NRTB project (https://*launchpad.net/nrtb).
 
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
 
#include <string>
#include <iostream>
#include <unistd.h>
#include "circular_queue.h"
#include <memory>
#include <future>

using namespace nrtb;
using namespace std;

typedef circular_queue<int> test_queue;
typedef std::shared_ptr<test_queue> queue_p;

int consumer_task(string name, queue_p input)
{
  bool ready {true};
  int count {0};
  try
  {
    while (ready)
    {
      int num = input->pop();
      count++;
      usleep(1);	  
    }
  }
  catch (...) 
  {
    // exit on any exception
    ready = false;
  };
  return count;
};

int main()
{
  cout << "***** circular_queue unit test *****" << endl;
  /************************************************
   * Load queue and then cook it down...
   ***********************************************/
  // make and load a queue
  queue_p q1(new test_queue(50));
  for (int i=0; i<100; i++)
  {
	q1->push(i);
  };
  // the queue should be loaded with 50-99
  // attach a thread and process it.
  auto t1 = async(launch::async,consumer_task,"task 1",q1);
  while (q1->size()) usleep(100);
  cout << "cp 1 " 
    << q1->in_count << ":"
    << q1->out_count << endl;
  /************************************************
   * now that the preload is exhasted, shove items
   * in one at a time to make sure each is picked
   * up correctly.
   ***********************************************/
  for (int i=200; i<225; i++)
  {
    q1->push(i);
    usleep(100);
  };
  cout << "cp 2 " 
    << q1->in_count << ":"
    << q1->out_count << endl;
  /************************************************
   * Last check; attach a second thread to the queue
   * and make sure both are servicing it.
   ***********************************************/
  auto t2 = async(launch::async,consumer_task,"task 2",q1);
  for (int i=300; i<325; i++)
  {
    q1->push(i);
  };
  while (q1->size()) usleep(100);
  // shut it all down
  q1->shutdown();
  // important numbers
  int q1_in = q1->in_count;
  int q1_out = q1->out_count;
  int t1_items = t1.get();
  int t2_items = t2.get();
  // release she threads and queues.
  q1.reset();
  // do some reporting.
  cout << "cp 3 " 
    << q1_in << ":" << q1_out
    << " t1=" << t1_items
    << " t2=" << t2_items 
    << endl;
  bool passed = (q1_in - 50 == q1_out)
    and (q1_out == (t1_items + t2_items));
  cout << "***** circular_queue TEST "
    << (passed ? "PASSED" : "FAILED")
    << " ******" << endl;
  // inverted logic needed because 0 is good for 
  // return codes.
  return !passed; 
};