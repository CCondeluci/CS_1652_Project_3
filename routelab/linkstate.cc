#include "linkstate.h"
#include <set>
#include "context.h"

LinkState::LinkState(unsigned n, SimulationContext* c, double b, double l) :
    Node(n, c, b, l)
{}

LinkState::LinkState(const LinkState & rhs) :
    Node(rhs)
{
    *this = rhs;
}

LinkState & LinkState::operator=(const LinkState & rhs) {
    Node::operator=(rhs);
    return *this;
}

LinkState::~LinkState() {}


// One of our outgoing links has been changed to the link represented by Link *l
void LinkState::LinkHasBeenUpdated(Link* l) {
    cerr << *this << ": Link Update: " << *l << endl;
    // Pull information about the new link from the Link object
    int src_node = l->GetSrc();
    int dest_node = l->GetDest();
    int new_latency = l->GetLatency();
    // Assign the new cost to the entry in the topology table
    routing_table.topo[src_node][dest_node].cost = new_latency;
    // increment the time the link came in
    routing_table.topo[src_node][dest_node].age++;
    int age = routing_table.topo[src_node][dest_node].age;
    // The table has been changed so Dijkstras should be run again
    Dijkstra();
    routing_table.changed = true;
    // Send this new link info to your neighbors
    SendToNeighbors(new RoutingMessage(age, src_node, dest_node, new_latency));
}

void LinkState::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    cerr << *this << " got a routing message: " << *m << endl;
    // Pull information from the message
    int age = m->link_age;
    int src = m->src_node;
    int dest = m->dest_node;
    int lat = m->new_latency;
    // if you do not have previous information about this link or this link is newer than your information
    if (routing_table.topo[src][dest].cost == -1 || 
        routing_table.topo[src][dest].age < age) {
      // update link info in the routing table
      routing_table.topo[src][dest].age = age;
      routing_table.topo[src][dest].cost = lat;
      Dijkstra();
      routing_table.changed = true;
      // Send this link to your neighbors
      SendToNeighbors(m);
    }
}

void LinkState::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* LinkState::GetNextHop(Node *destination) { 
      std::cout << "\nGETTING NEXT HOP" << std::endl;
      std::cout << destination->GetNumber() << std::endl;
      /*if (routing_table.changed == true) {
        std::cout << "Dijkstra is being run" << std::endl;
        Dijkstra();
        routing_table.changed = false;
      }*/
      // get next hop from the edgeTo array
      int step = destination->GetNumber();
      while ((unsigned)routing_table.edgeTo[step] != number) {
        step = routing_table.edgeTo[step];
      }
      Node *a = new Node(step, NULL, 0, 0);
      std::cout << "step: " << step << std::endl;
      //Node *a = new Node(routing_table.edgeTo[destination->GetNumber()], NULL, 0, 0);
      Node * realNode = context->FindMatchingNode(const_cast<Node *>(a));
      return realNode;      
}

Table* LinkState::GetRoutingTable() {
    Table *temp = new Table(routing_table);
    return temp;
}

void LinkState::Dijkstra() {
    map<int, map<int, TopoLink> >::const_iterator topo_itr;
    for (topo_itr = routing_table.topo.begin(); topo_itr != routing_table.topo.end(); topo_itr++) {
      routing_table.topo[topo_itr->first][topo_itr->first].cost = 0;
    }
    std::cout << "Starting Dijkstra: " << number << std::endl;
    std::cout << routing_table << std::endl;
    map<int, int> dist;
    map<int, int>::const_iterator dist_itr;
    map<int, int> prev;
    set<int> visited;
    int size = routing_table.topo.size();
   
    // Set distances
    for (int i = 0; (unsigned)i < routing_table.topo.size(); i++) {
      dist[i] = MAX_INT;
      prev[i] = -1;
    }
    dist[number] = 0; 
    prev[number] = number;
    for (int i = 0; i < size - 1; i++) {
      // Distance Array
      for (dist_itr = dist.begin(); dist_itr != dist.end(); dist_itr++) {
        std::cout << dist_itr->first << " : " << dist_itr->second << std::endl;
      }
      // Find minimum node
      int u = -1;
      int min = MAX_INT;
      std::cout << "Finding the minimum node:\n";
      for (topo_itr = routing_table.topo.begin(); topo_itr != routing_table.topo.end(); topo_itr++) {
        std::cout << "Looking at node: " << topo_itr->first << std::endl;
        if (visited.count(topo_itr->first) < 1 && dist[topo_itr->first] <= min) {
          min = dist[topo_itr->first];
          u = topo_itr->first;
        }
      }
      std::cout << "Relaxing node: " << u << std::endl;
      visited.insert(u);
      for (topo_itr = routing_table.topo.begin(); topo_itr != routing_table.topo.end(); topo_itr++) {
        int v = topo_itr->first;
        if (visited.count(v) < 1 &&
            routing_table.topo[u][v].cost != -1 &&
            dist[u] != MAX_INT &&
            dist[u] + routing_table.topo[u][v].cost < dist[v]) {
          dist[v] = dist[u] + routing_table.topo[u][v].cost;
          prev[v] = u;
        }
      }
    }
    for (dist_itr = prev.begin(); dist_itr != prev.end(); dist_itr++) {
      std::cout << dist_itr->first << " : " << dist_itr->second << std::endl;
    }
    routing_table.edgeTo = prev;
}

ostream & LinkState::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}
