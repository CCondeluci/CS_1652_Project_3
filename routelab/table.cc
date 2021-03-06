#include "table.h"

Table::Table() {
    topo.clear();
}

Table::Table(const Table & rhs) {
    *this = rhs;
}

Table & Table::operator=(const Table & rhs) {
    topo = rhs.topo;

    #if defined(LINKSTATE)
      edgeTo = rhs.edgeTo;
    #endif
    #if defined(DISTANCEVECTOR)
      neighborLinks = rhs.neighborLinks;
      distanceVector = rhs.distanceVector;
      edgeTo = rhs.edgeTo;
    #endif
    return *this;
}

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  os << "Generic Table()";
  return os;
}
#endif

#if defined(LINKSTATE)
ostream & Table::Print(ostream &os) const
{
  os << "\nLinkState Table:" << std::endl;
  
  /*os << "Link: " << std::endl;
  map<int, int>::const_iterator iter = edgeTo.begin();
  while(iter != edgeTo.end()) {
    
    os << "Node " << (*iter).first << " to Node " << (*iter).second << std::endl;
    iter++;
  }*/
map<int, map<int, TopoLink> >::const_iterator row = topo.begin();
    while (row != topo.end()) {
        map<int, TopoLink>::const_iterator col = (*row).second.begin();
        os << "Src=" << (*row).first << std::endl;

        while (col != (*row).second.end()) {
            os << "  Dest=" << (*col).first << " cost=" <<             (*col).second.cost<< " age=" << (*col).second.age << endl;
            col++;
        }
        row++;
    }
  return os;
}
#endif

#if defined(DISTANCEVECTOR)
ostream & Table::Print(ostream &os) const
{
    os << "\nDistance Vector Routing Table: " << std::endl;

    os << "Distance Vector: " << std::endl;
    map<int, TopoLink>::const_iterator iter = distanceVector.begin();
    while (iter != distanceVector.end()) {
      os << "Node: " << (*iter).first << " Dist: " << (*iter).second.cost << std::endl;
      iter++;
    }
    os << "\nNeighbor List: " << std::endl;
    iter = neighborLinks.begin();
    while (iter != neighborLinks.end()) {
      os << "Node: " << (*iter).first << " Dist: " << (*iter).second.cost << std::endl;
      iter++;
    }
    /*
    map<int, map<int, TopoLink> >::const_iterator row = topo.begin();
    while (row != topo.end()) {
        map<int, TopoLink>::const_iterator col = (*row).second.begin();
        os << "Src=" << (*row).first << std::endl;

        while (col != (*row).second.end()) {
            os << "  Dest=" << (*col).first << " cost=" <<             (*col).second.cost<< " age=" << (*col).second.age << endl;
            col++;
        }
        row++;
    }*/

    return os;
}
#endif
