// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <array>
#include <vector>
#include <map>
#include "geometryUtils.h"

namespace agp
{
    class Object;
    class Quadtree;
}

// Quadtree (standard/scrict) class
// - container for game objects stored according to spatial partitioning
// - objects are stored in the smallest node that entirely contains its rect
// - provides efficient spatial queries for game scenes

class agp::Quadtree
{
    private:

        // parameters
        static constexpr int MAX_OBJECTS_PER_NODE = 16;
        static constexpr int MAX_DEPTH = 8;

        // inner classes/structs
        struct Node;

        // attributes
        RectF _rect;
        Node* _root;
        std::map<int, Node*> _objectToNode;

        // helper functions
        bool isLeaf(const Node* node) const;
        RectF indexToQuadrant(const RectF& rect, int i) const;
        int getObjectQuadrant(const RectF& quadRect, const RectF& objRect) const;
        void add(Node* node, std::size_t depth, const RectF& nodeRect, Object* obj);
        void split(Node* node, const RectF& nodeRect);
        //bool remove(Node* node, const RectF& nodeRect, Object* obj);
        void nodeAddObject(Node* node, Object* obj);
        void nodeRemoveObject(Node* node, Object* obj);
        bool tryMerge(Node* node);
        void query(Node* node, const RectF& nodeRect, const RectF& queryRect, std::vector<Object*>& objects) const;
        void queryIntersections(Node* node, std::vector<std::pair<Object*, Object*>>& intersections) const;
        void queryIntersectionsInDescendants(Node* node, Object* obj, std::vector<std::pair<Object*, Object*>>& intersections) const;

    public:

        Quadtree(const RectF& rect);
        RectF rect() const { return _rect; }

        void add(Object* obj);
        void remove(Object* obj);
        void update(Object* obj);

        std::vector<Object*> queryObjects(const RectF& rect) const;
        std::vector<std::pair<Object*, Object*>> queryIntersections() const;
};
