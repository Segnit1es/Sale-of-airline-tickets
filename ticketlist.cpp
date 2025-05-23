#include "ticketlist.h"
#include <vector>
#include <stdexcept>

TicketList::TicketList(ErrorHandler *handler)
    : head(nullptr), tail(nullptr),errorHandler(handler) {}

TicketList::~TicketList() {
    Clear();
}

void TicketList::Append(const TicketRecord& rec) {
    TicketNode* node = new TicketNode(rec);
    if (!head) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
}

void TicketList::FindMinMax(int& minKey, int& maxKey) const {
    if (!head) {
        if (errorHandler)
            errorHandler->RaiseError("Список билетов пуст, невозможно определить минимум и максимум.",
                                     "TicketList::FindMinMax");
        minKey = maxKey = 0;
        return;
    }

    minKey = std::numeric_limits<int>::max();
    maxKey = std::numeric_limits<int>::min();
    for (TicketNode* cur = head; cur; cur = cur->next) {
        int key = TicketNumberToInt(cur->data.GetTicketNumber());
        if (key < minKey) minKey = key;
        if (key > maxKey) maxKey = key;
    }
}
int TicketList::TicketNumberToInt(const QString& ticketNumber) const {
    bool ok = false;
    int val = ticketNumber.toInt(&ok);
    if (!ok) {
        if (errorHandler)
            errorHandler->RaiseError(
                "Некорректный формат номера билета: \"" + ticketNumber + "\".",
                "TicketList::TicketNumberToInt"
                );
        return 0;
    }
    return val;
}

void TicketList::SortByTicketNumber() {
    if (!head || head == tail) return; // 0 или 1 элемент

    int minKey, maxKey;
    FindMinMax(minKey, maxKey);
    int range = maxKey - minKey + 1;

    // Создаём вектор счётчиков списков
    std::vector<TicketNode*> bucketHeads(range, nullptr);
    std::vector<TicketNode*> bucketTails(range, nullptr);

    // Разбиваем на «корзины»
    for (TicketNode* cur = head; cur; ) {
        int key = TicketNumberToInt(cur->data.GetTicketNumber()) - minKey;
        TicketNode* next = cur->next;
        cur->next = nullptr;
        if (!bucketHeads[key]) {
            bucketHeads[key] = bucketTails[key] = cur;
        } else {
            bucketTails[key]->next = cur;
            bucketTails[key] = cur;
        }
        cur = next;
    }

    // Собираем обратно в один список
    head = tail = nullptr;
    for (int i = 0; i < range; ++i) {
        if (bucketHeads[i]) {
            if (!head) {
                head = bucketHeads[i];
                tail = bucketTails[i];
            } else {
                tail->next = bucketHeads[i];
                tail = bucketTails[i];
            }
        }
    }
}

void TicketList::Clear() {
    TicketNode* cur = head;
    while (cur) {
        TicketNode* next = cur->next;
        delete cur;
        cur = next;
    }
    head = tail = nullptr;
}

void TicketList::Traverse(std::function<void(const TicketRecord&)> callback) const {
    for (TicketNode* cur = head; cur; cur = cur->next) {
        callback(cur->data);
    }
}
bool TicketList::RemoveByTicketNumber(const QString &ticketNumber,
                                      std::function<void(const TicketRecord&)> onRemove)
{
    TicketNode* prev = nullptr;
    TicketNode* cur = head;
    while (cur) {
        if (cur->data.GetTicketNumber() == ticketNumber) {
            // вызов колбэка до удаления
            if (onRemove) onRemove(cur->data);

            // удаляем узел
            if (prev) {
                prev->next = cur->next;
            } else {
                head = cur->next;
            }
            if (cur == tail) {
                tail = prev;
            }
            delete cur;
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}
