/*
    Copyright (c) 2015, Damian Barczynski <daan.net@wp.eu>
    Following tool is licensed under the terms and conditions of the ISC license.
    For more information visit https://opensource.org/licenses/ISC.
*/
#ifndef __ASTAR_HPP_8F637DB91972F6C878D41D63F7E7214F__
#define __ASTAR_HPP_8F637DB91972F6C878D41D63F7E7214F__

#include <vector>
#include <functional>
#include <set>
#include <memory>
#include "Helper.h"

namespace AStar
{
    using uint = unsigned int;
    using HeuristicFunction = std::function<uint(Point, Point)>;
    using CoordinateList = std::vector<Point>;

    struct Node
    {
        uint G, H;
        Point coordinates;
        Node *parent;

        Node(Point coord_, Node *parent_ = nullptr);
        uint getScore();
    };

    using NodeSet = std::vector<Node*>;

    class Generator
    {
        bool checkWall(int x, int y);
        bool detectCollision(Point previousCoordinates_, Point direction_, Point coordinates_);
        Node* findNodeOnList(NodeSet& nodes_, Point coordinates_);
        void releaseNodes(NodeSet& nodes_);

    public:
        Generator();
        void setWorldSize(Point worldSize_);
        void setDiagonalMovement(bool enable_);
        void setHeuristic(HeuristicFunction heuristic_);
        CoordinateList findPath(Point source_, Point target_, int maxIterations_ = 300);
        void addWall(Point coordinates_);
        void addMonster(Point coordinates1, Point coordinates2);
        void removeMonster(Point coordinates_);
        void clearMonsters();

    private:
        HeuristicFunction heuristic;
        CoordinateList direction, monsters;
        Point worldSize;
        std::unique_ptr<bool[]> walls;
        uint directions;
    };

    class Heuristic
    {
        static Point getDelta(Point source_, Point target_);

    public:
        static uint manhattan(Point source_, Point target_);
        static uint euclidean(Point source_, Point target_);
        static uint octagonal(Point source_, Point target_);
    };
}

#endif // __ASTAR_HPP_8F637DB91972F6C878D41D63F7E7214F__
