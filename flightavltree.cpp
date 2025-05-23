#include "flightavltree.h"

FlightAVLTree::FlightAVLTree() : root(nullptr) {}
FlightAVLTree::~FlightAVLTree() { Clear(); }

int FlightAVLTree::Height(FlightNode* node) const {
    return node ? node->height : 0;
}

int FlightAVLTree::BalanceFactor(FlightNode* node) const {
    return node ? Height(node->right) - Height(node->left) : 0;
}

void FlightAVLTree::UpdateHeight(FlightNode* node) {
    node->height = 1 + std::max(Height(node->left), Height(node->right));
}

FlightNode* FlightAVLTree::RotateRight(FlightNode* y) {
    FlightNode* x = y->left;
    FlightNode* T2 = x->right;
    // Поворот
    x->right = y;
    y->left = T2;
    // Обновляем высоты
    UpdateHeight(y);
    UpdateHeight(x);
    return x;
}

FlightNode* FlightAVLTree::RotateLeft(FlightNode* x) {
    FlightNode* y = x->right;
    FlightNode* T2 = y->left;
    // Поворот
    y->left = x;
    x->right = T2;
    // Обновляем высоты
    UpdateHeight(x);
    UpdateHeight(y);
    return y;
}

bool FlightAVLTree::Insert(std::shared_ptr<Flight> flight) {
    bool inserted = false;
    root = InsertNode(root, std::move(flight), inserted);
    return inserted;
}

FlightNode* FlightAVLTree::InsertNode(FlightNode* node, std::shared_ptr<Flight> flight, bool& inserted) {
    if (!node) {
        inserted = true;
        return new FlightNode(std::move(flight));
    }
    const QString& key = flight->GetFlightNumber();
    if (key == node->data->GetFlightNumber()) {
        inserted = false;
        return node;
    }
    if (key < node->data->GetFlightNumber())
        node->left  = InsertNode(node->left, std::move(flight), inserted);
    else
        node->right = InsertNode(node->right, std::move(flight), inserted);

    UpdateHeight(node);
    int bf = BalanceFactor(node);

    // LL
    if (bf < -1 && key < node->left->data->GetFlightNumber())
        return RotateRight(node);
    // RR
    if (bf > 1 && key > node->right->data->GetFlightNumber())
        return RotateLeft(node);
    // LR
    if (bf < -1 && key > node->left->data->GetFlightNumber()) {
        node->left = RotateLeft(node->left);
        return RotateRight(node);
    }
    // RL
    if (bf > 1 && key < node->right->data->GetFlightNumber()) {
        node->right = RotateRight(node->right);
        return RotateLeft(node);
    }
    return node;
}

std::shared_ptr<Flight> FlightAVLTree::Find(const QString& flightNumber) const {
    return FindNode(root, flightNumber);
}

std::shared_ptr<Flight> FlightAVLTree::FindNode(FlightNode* node, const QString& key) const {
    if (!node) return nullptr;
    if (key == node->data->GetFlightNumber())
        return node->data;
    if (key < node->data->GetFlightNumber())
        return FindNode(node->left, key);
    else
        return FindNode(node->right, key);
}

void FlightAVLTree::TraverseReverse(std::function<void(const std::shared_ptr<Flight>&)> callback) const {
    TraverseReverseNode(root, std::move(callback));
}

void FlightAVLTree::TraverseReverseNode(FlightNode* node, std::function<void(const std::shared_ptr<Flight>&)> callback) const {
    if (!node) return;
    // Сначала левое поддерево
    TraverseReverseNode(node->left, callback);
    // Затем правое поддерево
    TraverseReverseNode(node->right, callback);
    // Корень
    callback(node->data);
}

void FlightAVLTree::DeleteSubtree(FlightNode* node) {
    if (!node) return;
    DeleteSubtree(node->left);
    DeleteSubtree(node->right);
    delete node;
}

void FlightAVLTree::Clear() {
    DeleteSubtree(root);
    root = nullptr;
}
bool FlightAVLTree::Remove(const QString &flightNumber) {
    bool removed = false;
    root = RemoveNode(root, flightNumber, removed);
    return removed;
}

FlightNode* FlightAVLTree::RemoveNode(FlightNode* node, const QString& key, bool& removed) {
    if (!node) return nullptr;

    if (key < node->data->GetFlightNumber()) {
        node->left = RemoveNode(node->left, key, removed);
    }
    else if (key > node->data->GetFlightNumber()) {
        node->right = RemoveNode(node->right, key, removed);
    }
    else {
        // нашли узел
        removed = true;
        // случай с одним/нулём ребёнком
        if (!node->left || !node->right) {
            FlightNode* temp = node->left ? node->left : node->right;
            delete node;
            return temp;
        }
        // два ребёнка: находим преемника (минимум в правом поддереве)
        FlightNode* succ = FindMinNode(node->right);
        node->data = succ->data;
        node->right = RemoveNode(node->right, succ->data->GetFlightNumber(), removed);
    }

    // если после удаления дерево пусто
    if (!node) return nullptr;

    // обновляем высоту и балансируем
    UpdateHeight(node);
    int bf = BalanceFactor(node);

    // LL
    if (bf < -1 && BalanceFactor(node->left) <= 0)
        return RotateRight(node);
    // LR
    if (bf < -1 && BalanceFactor(node->left) > 0) {
        node->left = RotateLeft(node->left);
        return RotateRight(node);
    }
    // RR
    if (bf > 1 && BalanceFactor(node->right) >= 0)
        return RotateLeft(node);
    // RL
    if (bf > 1 && BalanceFactor(node->right) < 0) {
        node->right = RotateRight(node->right);
        return RotateLeft(node);
    }

    return node;
}

FlightNode* FlightAVLTree::FindMinNode(FlightNode* node) const {
    while (node && node->left)
        node = node->left;
    return node;
}
