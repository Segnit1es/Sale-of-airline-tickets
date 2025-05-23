#ifndef FLIGHTAVLTREE_H
#define FLIGHTAVLTREE_H

#include "flightnode.h"
#include <functional>
#include <vector>

class FlightAVLTree {
public:
    FlightAVLTree();
    ~FlightAVLTree();

    // Вставка нового рейса. Вернёт false, если flightNumber уже есть
    bool Insert(std::shared_ptr<Flight> flight);

    // Поиск рейса по номеру
    std::shared_ptr<Flight> Find(const QString& flightNumber) const;

    // Обратный обход: передаёт каждый Flight в callback в порядке убывания
    void TraverseReverse(std::function<void(const std::shared_ptr<Flight>&)> callback) const;

    // Очистить дерево
    void Clear();
    bool Remove(const QString &flightNumber);

private:
    FlightNode* root;

    // Вспомогательные методы
    int  Height(FlightNode* node) const;
    int  BalanceFactor(FlightNode* node) const;
    void UpdateHeight(FlightNode* node);

    FlightNode* RotateRight(FlightNode* y);
    FlightNode* RotateLeft(FlightNode* x);

    FlightNode* InsertNode(FlightNode* node, std::shared_ptr<Flight> flight, bool& inserted);
    std::shared_ptr<Flight> FindNode(FlightNode* node, const QString& key) const;
    void TraverseReverseNode(FlightNode* node, std::function<void(const std::shared_ptr<Flight>&)> callback) const;
    void DeleteSubtree(FlightNode* node);
    FlightNode* RemoveNode(FlightNode* node, const QString& key, bool& removed);
    FlightNode* FindMinNode(FlightNode* node) const;
};

#endif // FLIGHTAVLTREE_H
