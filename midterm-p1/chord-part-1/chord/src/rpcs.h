#ifndef RPCS_H
#define RPCS_H

#include "chord.h"
#include "rpc/client.h"
#define MOD 1024

Node self, successor, predecessor;

Node get_info() { return self; } // Do not modify this line.

bool all_in_ring_clockwise_order(uint64_t prev, uint64_t middle, uint64_t next, bool back_equal = false)
{
  if (prev < middle && middle < next)
  {
    return true;
  }
  if (middle < next && next < prev)
  {
    return true;
  }
  if (next < prev && prev < middle)
  {
    return true;
  }
  if (back_equal)
  {
    if (prev == middle || prev == next || middle == next)
    {
      return true;
    }
  }
  return false;
}

void create()
{
  // point the predecessor to nil and the successor to itself
  predecessor.ip = "";
  successor = self;
}

void join(Node n)
{
  predecessor.ip = "";
  rpc::client client(n.ip, n.port);
  successor = client.call("find_successor", self.id).as<Node>();
}

Node get_predecessor()
{
  return predecessor;
}

Node find_successor(uint64_t id)
{
  std::cout << "find_successor: " << id << "from node: " << self.id << std::endl;
  // TODO: implement your `find_successor` RPC
  if (all_in_ring_clockwise_order(self.id % MOD, id % MOD, successor.id % MOD, true))
  {
    return successor;
  }
  else
  {
    rpc::client client(successor.ip, successor.port);
    return client.call("find_successor", id).as<Node>();
  }
}

void stabilize()
{
  if (successor.ip == "")
  {
    return;
  }
  rpc::client client(successor.ip, successor.port);
  Node x = client.call("get_predecessor").as<Node>();
  if (x.ip != "" && all_in_ring_clockwise_order(self.id % MOD, x.id % MOD, successor.id % MOD))
  {
    successor = x;
  }
  rpc::client new_client(successor.ip, successor.port);
  new_client.call("notify", self);
}

void notify(Node n)
{
  if (predecessor.ip == "" || all_in_ring_clockwise_order(predecessor.id % MOD, n.id % MOD, self.id % MOD))
  {
    predecessor = n;
  }
}

void check_predecessor()
{
  try
  {
    rpc::client client(predecessor.ip, predecessor.port);
    Node n = client.call("get_info").as<Node>();
  }
  catch (std::exception &e)
  {
    predecessor.ip = "";
  }
}

void register_rpcs()
{
  add_rpc("get_info", &get_info); // Do not modify this line.
  add_rpc("create", &create);
  add_rpc("join", &join);
  add_rpc("find_successor", &find_successor);
  add_rpc("get_predecessor", &get_predecessor);
  add_rpc("notify", &notify);
}

void register_periodics()
{
  add_periodic(check_predecessor);
  add_periodic(stabilize);
}

#endif /* RPCS_H */
