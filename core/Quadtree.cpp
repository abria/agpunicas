// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Quadtree.h"
#include <algorithm>
#include <utility>
#include <iostream>
#include "Object.h"

using namespace agp;

struct Quadtree::Node
{
    std::array<Node*, 4> children;
    std::vector<Object*> objects;

    Node()
    {
        for (int i = 0; i < 4; i++)
            children[i] = nullptr;
    }

    ~Node()
    {
        for (auto child : children)
            delete child;
    }
};

Quadtree::Quadtree(const RectF& rect) :
    _rect(rect)
{
    _root = new Node();
}

Quadtree::~Quadtree()
{
    delete _root;
}

void Quadtree::add(Object* obj)
{
    if (!_rect.contains(obj->rect()))
    {
        if (VERBOSE)
            std::cerr << strprintf("Quadtree::add: quadtree rect (%s) does not fully contain object (%s) rect (%s)", _rect.str().c_str(), obj->name().c_str(), obj->rect().str().c_str());
        return;
    }

    add(_root, 0, _rect, obj);
}

void Quadtree::remove(Object* obj)
{
    auto it = _objectToNode.find(obj->id());
    if (it != _objectToNode.end())
        nodeRemoveObject(it->second, obj);
}

void Quadtree::update(Object* obj)
{
    if (_objectToNode.find(obj->id()) == _objectToNode.end())
    {
        if(VERBOSE)
            std::cerr << "Quadtree::update: trying to update an object [" << obj->name() << ", rect = " << obj->rect().str() << "] that is not present in the quadtree\n";
        add(obj);
        return;
    }

    remove(obj);
    add(obj);
}

std::vector<Object*> Quadtree::queryObjects(const RectF& queryRect) const
{
    auto objects = std::vector<Object*>();
    query(_root, _rect, queryRect, objects);

    return objects;
}

bool Quadtree::isLeaf(const Node* node) const
{
    return !static_cast<bool>(node->children[0]);
}

RectF Quadtree::indexToQuadrant(const RectF& rect, int i) const
{
    PointF quadSize = rect.size / static_cast<float>(2);

    switch (i)
    {
        case 0:
            return RectF(rect.pos, rect.pos + quadSize, rect.yUp);
        case 1:
            return RectF(PointF(rect.pos.x + quadSize.x, rect.pos.y), PointF(rect.pos.x + quadSize.x, rect.pos.y) + quadSize, rect.yUp);
        case 2:
            return RectF(PointF(rect.pos.x, rect.pos.y + quadSize.y), PointF(rect.pos.x, rect.pos.y + quadSize.y) + quadSize, rect.yUp);
        case 3:
            return RectF(rect.pos + quadSize, rect.pos + 2 * quadSize, rect.yUp);
        default:
            throw "Quadtree::indexToQuadrant: invalid child index";
    }
}

int Quadtree::getObjectQuadrant(const RectF& quadRect, const RectF& objRect) const
{
    for (int i = 0; i < 4; i++)
        if (indexToQuadrant(quadRect, i).contains(objRect))
            return i;

    return -1;
}

void Quadtree::add(Node* node, std::size_t depth, const RectF& nodeRect, Object* obj)
{
    if(!node)
        throw "Quadtree::add: invalid node pointer";

    if(!nodeRect.contains(obj->rect()))
        throw strprintf("Quadtree::add: nodeRect (%s) does not fully contain object (%s) rect (%s)", nodeRect.str().c_str(), obj->name().c_str(), obj->rect().str().c_str());

    if (isLeaf(node))
    {
        // insert in this node if necessary (max depth exceeded) or possible (max objects not exceeded)
        if (depth >= MAX_DEPTH || node->objects.size() < MAX_OBJECTS_PER_NODE)
            nodeAddObject(node, obj);
        // otherwise, split and try again
        else
        {
            split(node, nodeRect);
            add(node, depth, nodeRect, obj);
        }
    }
    else
    {
        int i = getObjectQuadrant(nodeRect, obj->rect());
        // add the value in a child if the value is entirely contained in it
        if (i != -1)
            add(node->children[i], depth + 1, indexToQuadrant(nodeRect, i), obj);
        // otherwise, add the value in the current node
        else
            nodeAddObject(node, obj);
    }
}

void Quadtree::split(Node* node, const RectF& nodeRect)
{
    if (!node)
        throw "Quadtree::split: invalid node pointer";

    if (!isLeaf(node))
        throw "Quadtree::split: only leaves can be split";

    // create children
    for (auto& child : node->children)
        child = new Node();

    // assign objects to children or to current node
    auto newObjects = std::vector<Object*>();
    for (const auto& obj : node->objects)
    {
        auto i = getObjectQuadrant(nodeRect, obj->rect());
        if (i != -1)
            nodeAddObject(node->children[i], obj);
        else
            newObjects.push_back(obj);
    }
    node->objects = std::move(newObjects);
}

void Quadtree::nodeAddObject(Node* node, Object* obj)
{
    auto it = std::find_if(std::begin(node->objects), std::end(node->objects),
        [this, &obj](const auto& rhs) { return obj->id() == rhs->id(); });

    if (it != std::end(node->objects))
    {
        if (VERBOSE)
            std::cerr << "Quadtree::nodeAddObject: trying to add an object already present in the node\n";
        return;
    }

    node->objects.push_back(obj);
    _objectToNode[obj->id()] = node;
}

void Quadtree::nodeRemoveObject(Node* node, Object* obj)
{
    auto it = std::find_if(std::begin(node->objects), std::end(node->objects),
        [this, &obj](const auto& rhs) { return obj->id() == rhs->id(); });
    
    if (it == std::end(node->objects))
    {
        if (VERBOSE)
            std::cerr << "Quadtree::nodeRemoveObject: trying to remove an object that is not present in the node\n";
        return;
    }

    // swap with the last element and pop back
    *it = std::move(node->objects.back());
    node->objects.pop_back();

    _objectToNode.erase(obj->id());
}

void Quadtree::query(Node* node, const RectF& nodeRect, const RectF& queryRect, std::vector<Object*>& objects) const
{
    if (!node)
    {
        std::cerr << "Quadtree::query: invalid node pointer\n";
        return;
    }

    if (!queryRect.intersects(nodeRect))
    {
        if(VERBOSE)
            std::cerr << strprintf("Quadtree::query: queryRect (%s) does not intersect nodeRect (%s)\n", queryRect.str().c_str(), nodeRect.str().c_str());
        return;
    }

    for (const auto& value : node->objects)
    {
        if (queryRect.intersects(value->rect()))
            objects.push_back(value);
    }
    if (!isLeaf(node))
    {
        for (int i = 0; i < 4; ++i)
        {
            RectF childRect = indexToQuadrant(nodeRect, i);
            if (queryRect.intersects(childRect))
                query(node->children[i], childRect, queryRect, objects);
        }
    }
}
