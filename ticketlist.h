#ifndef TICKETLIST_H
#define TICKETLIST_H

#include "TicketNode.h"
#include <limits>

class TicketList {
public:
    TicketList(ErrorHandler *handler);
    ~TicketList();

    // Добавить запись в конец списка (без сортировки)
    void Append(const TicketRecord& rec);

    // Упорядочить список по ticketNumber методом подсчётом
    void SortByTicketNumber();

    // Удалить все элементы
    void Clear();

    // Пройтись по списку и вызвать callback для каждой записи
    void Traverse(std::function<void(const TicketRecord&)> callback) const;

    bool RemoveByTicketNumber(const QString &ticketNumber,
                              std::function<void(const TicketRecord&)> onRemove = nullptr);

private:
    TicketNode* head;
    TicketNode* tail;
    ErrorHandler *errorHandler;

    // Вспомогательное: получить минимум и максимум ticketNumber в виде int
    void FindMinMax(int& minKey, int& maxKey) const;
    // Конвертация номера билета "NNNNNNNNN" → int
    int TicketNumberToInt(const QString& ticketNumber) const;
};

#endif // TICKETLIST_H
