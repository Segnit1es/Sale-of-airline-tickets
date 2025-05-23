#ifndef TICKETNODE_H
#define TICKETNODE_H

#include "ticketrecord.h"

struct TicketNode {
    TicketRecord data;
    TicketNode* next;

    TicketNode(const TicketRecord& rec)
        : data(rec), next(nullptr) {}
};

#endif // TICKETNODE_H
