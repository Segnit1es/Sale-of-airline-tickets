#ifndef FLIGHTNODE_H
#define FLIGHTNODE_H

#include <memory>
#include "flight.h"

struct FlightNode {
    std::shared_ptr<Flight> data;
    FlightNode* left  = nullptr;
    FlightNode* right = nullptr;
    int height = 1;

    FlightNode(std::shared_ptr<Flight> flight)
        : data(std::move(flight)) {}
};

#endif // FLIGHTNODE_H
