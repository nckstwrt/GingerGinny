#include "AStar.h"
#include <algorithm>
#include <math.h>

using namespace std::placeholders;

AStar::Node::Node(Point coordinates_, Node *parent_)
{
    parent = parent_;
    coordinates = coordinates_;
    G = H = 0;
}

AStar::uint AStar::Node::getScore()
{
    return G + H;
}

AStar::Generator::Generator()
{
    setDiagonalMovement(false);
    setHeuristic(&Heuristic::manhattan);
    direction = {
        { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
        { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
    };
}

void AStar::Generator::setWorldSize(Point worldSize_)
{
    worldSize = worldSize_;
    walls = std::make_unique<bool[]>(worldSize.x * worldSize.y);
}

void AStar::Generator::setDiagonalMovement(bool enable_)
{
    directions = (enable_ ? 8 : 4);
}

void AStar::Generator::setHeuristic(HeuristicFunction heuristic_)
{
    heuristic = std::bind(heuristic_, _1, _2);
}

void AStar::Generator::addWall(Point coordinates_)
{
    walls[coordinates_.x + (coordinates_.y * worldSize.x)] = true;
}

void AStar::Generator::addMonster(Point coordinates1, Point coordinates2)
{
    for (int y = coordinates1.y; y <= coordinates2.y; y++)
        for (int x = coordinates1.x; x <= coordinates2.x; x++)
            monsters.push_back({ x, y });
}

void AStar::Generator::removeMonster(Point coordinates_)
{
    auto it = std::find(monsters.begin(), monsters.end(), coordinates_);
    if (it != monsters.end()) {
        monsters.erase(it);
    }
}

void AStar::Generator::clearMonsters()
{
    monsters.clear();
}

bool AStar::Generator::checkWall(int x, int y)
{
    return walls[x + (y * worldSize.x)];
}

bool AStar::Generator::detectCollision(Point previousCoordinates_, Point direction_, Point coordinates_)
{
    if (coordinates_.x < 0 || coordinates_.x >= worldSize.x ||
        coordinates_.y < 0 || coordinates_.y >= worldSize.y || 
        walls[coordinates_.x + (coordinates_.y * worldSize.x)] ||
        std::find(monsters.begin(), monsters.end(), coordinates_) != monsters.end()) {
        return true;
    }

    // Check diagonal and walls in between
    // Going Down and to the Left/Right
    if ((direction_.x == 1 || direction_.x == -1) && (direction_.y == 1 || direction_.y == -1))
    {
        // check if there's a wall beneath us
        if (checkWall(previousCoordinates_.x, previousCoordinates_.y + direction_.y))
            return true;
        if (checkWall(previousCoordinates_.x + direction_.x, previousCoordinates_.y))
            return true;
    }

    return false;
}

AStar::CoordinateList AStar::Generator::findPath(Point source_, Point target_, int maxIterations_)
{
    Node *current = nullptr;
    NodeSet openSet, closedSet;
    openSet.reserve(100);
    closedSet.reserve(100);
    openSet.push_back(new Node(source_));

    int iterations = 0;
    while (!openSet.empty()) {
        iterations++;
        if (iterations >= maxIterations_)
            break;
        auto current_it = openSet.begin();
        current = *current_it;

        for (auto it = openSet.begin(); it != openSet.end(); it++) {
            auto node = *it;
            if (node->getScore() <= current->getScore()) {
                current = node;
                current_it = it;
            }
        }

        if (current->coordinates == target_) {
            break;
        }

        closedSet.push_back(current);
        openSet.erase(current_it);

        for (uint i = 0; i < directions; ++i) {
            Point newCoordinates(current->coordinates + direction[i]);

            if ((!(newCoordinates == target_) && detectCollision(current->coordinates, direction[i], newCoordinates)) ||
                findNodeOnList(closedSet, newCoordinates)) {
                continue;
            }

            uint totalCost = current->G + ((i < 4) ? 10 : 14);

            Node *successor = findNodeOnList(openSet, newCoordinates);
            if (successor == nullptr) {
                successor = new Node(newCoordinates, current);
                successor->G = totalCost;
                successor->H = heuristic(successor->coordinates, target_);
                openSet.push_back(successor);
            }
            else if (totalCost < successor->G) {
                successor->parent = current;
                successor->G = totalCost;
            }
        }
    }

    CoordinateList path;
    while (current != nullptr) {
        path.push_back(current->coordinates);
        current = current->parent;
    }

    releaseNodes(openSet);
    releaseNodes(closedSet);

    return path;
}

AStar::Node* AStar::Generator::findNodeOnList(NodeSet& nodes_, Point coordinates_)
{
    for (auto node : nodes_) {
        if (node->coordinates == coordinates_) {
            return node;
        }
    }
    return nullptr;
}

void AStar::Generator::releaseNodes(NodeSet& nodes_)
{
    for (auto it = nodes_.begin(); it != nodes_.end();) {
        delete *it;
        it = nodes_.erase(it);
    }
}

Point AStar::Heuristic::getDelta(Point source_, Point target_)
{
    return{ abs(source_.x - target_.x),  abs(source_.y - target_.y) };
}

AStar::uint AStar::Heuristic::manhattan(Point source_, Point target_)
{
    auto delta = std::move(getDelta(source_, target_));
    return static_cast<uint>(10 * (delta.x + delta.y));
}

AStar::uint AStar::Heuristic::euclidean(Point source_, Point target_)
{
    auto delta = std::move(getDelta(source_, target_));
    return static_cast<uint>(10 * sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
}

AStar::uint AStar::Heuristic::octagonal(Point source_, Point target_)
{
    auto delta = std::move(getDelta(source_, target_));
    return 10 * (delta.x + delta.y) + (-6) * std::min(delta.x, delta.y);
}
