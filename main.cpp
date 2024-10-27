#include "Graph.hpp"
#include "MSTStrategy.hpp"
#include "MSTree.hpp"
#include <iostream>
#include <memory>
#include "pollserver.hpp"
using namespace std;

#define PORT "9034"

int main()
{
    poll_clients(PORT);
    return 0;
}
