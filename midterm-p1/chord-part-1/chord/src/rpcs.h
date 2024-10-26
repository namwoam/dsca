#ifndef RPCS_H
#define RPCS_H

#include "chord.h"
#include "rpc/client.h"
#define MOD 1024
#define FINGER_TABLE_SIZE 10

Node self, successor, predecessor;

Node get_info() { return self; } // Do not modify this line.

std::vector<Node> finger_table(FINGER_TABLE_SIZE);

bool all_in_ring_clockwise_order(uint64_t prev, uint64_t middle, uint64_t next, bool back_equal = false);
void update_finger_table();
Node closest_preceding_node(uint64_t id);
void create();
void join(Node n);
Node get_predecessor();
Node find_successor(uint64_t id);
void stabilize();
void notify(Node n);
void check_predecessor();

bool all_in_ring_clockwise_order(uint64_t prev, uint64_t middle, uint64_t next, bool back_equal)
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

void update_finger_table()
{
  for (int i = 0; i < finger_table.size(); i++)
  {
    uint64_t start = self.id + (1 << i);
    finger_table.at(i) = find_successor(start);
  }
}

void show_status()
{
  std::cout << "Node " << self.id << " has successor " << successor.id << " and predecessor " << predecessor.id << std::endl;
  std::cout << "Finger table of node " << self.id << ":" << std::endl;
  for (int i = 0; i < finger_table.size(); i++)
  {
    std::cout << "Finger " << i << ": " << finger_table.at(i).id << std::endl;
  }
}

Node closest_preceding_node(uint64_t id)
{
  for (int i = finger_table.size() - 1; i >= 0; i--)
  {
    if (all_in_ring_clockwise_order(self.id % MOD, finger_table.at(i).id % MOD, id % MOD))
    {
      return finger_table.at(i);
    }
  }
  return self;
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
  Node result;
  // TODO: implement your `find_successor` RPC
  if (all_in_ring_clockwise_order(self.id % MOD, id % MOD, successor.id % MOD, true))
  {
    return successor;
  }
  else
  {
    Node n = closest_preceding_node(id);
    rpc::client client(n.ip, n.port);
    try
    {
      client.call("get_info").as<Node>();
    }
    catch (std::exception &e)
    {
      return self;
    }
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
  Node x;
  try
  {
    client.call("get_info").as<Node>();
  }
  catch (std::exception &e)
  {
    successor.ip = "";
    return;
  }
  x = client.call("get_predecessor").as<Node>();
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
  add_periodic(update_finger_table);
  add_periodic(show_status);
}

#endif /* RPCS_H */
