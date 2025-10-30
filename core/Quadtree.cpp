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
#include <memory>
#include <iostream>
#include "Object.h"

using namespace agp;

struct Quadtree::Node
{
    std::array<Node*, 4> children;
    std::vector<Object*> objects;

    Node()
    {
        int count = 0;
        for (int i = 0; i < 4; i++)
            children[i] = nullptr;
    }
};

Quadtree::Quadtree(const RectF& rect) :
    _rect(rect)
{
    _root = new Node();
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
    //remove(_root, _rect, obj);
    if (_objectToNode.find(obj->id()) != _objectToNode.end())
        nodeRemoveObject(_objectToNode[obj->id()], obj);
}

void Quadtree::update(Object* obj)
{
    if (_objectToNode.find(obj->id()) == _objectToNode.end())
    {
        if(VERBOSE)
            std::cerr << "Quadtree::update: trying to update an object [" << obj->name() << ", rect = " << obj->rect().str() << "] that is not present in the quadtree\n";
        return;
    }

    remove(obj);
    add(obj);
}

std::vector<Object*> Quadtree::queryObjects(const RectF& queryRect) const
{
    auto objects = std::vector<Object*>();
    query(_root, _rect, queryRect, objects);

    /*printf("\n_rect = %s\nqueryRect = %s\nobjects = %d\n", 
        _rect.str().c_str(), queryRect.str().c_str(), objects.size());
    printf("{\n");
    for (auto v : objects)
        printf("\t%s at %s\n", v->name().c_str(), v->rect().str().c_str());
    printf("}\n");*/

    return objects;
}

std::vector<std::pair<Object*, Object*>> Quadtree::queryIntersections() const
{
    auto intersections = std::vector<std::pair<Object*, Object*>>();
    queryIntersections(_root, intersections);
    return intersections;
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

    if (it != std::end(node->objects) && VERBOSE)
        std::cerr << "Quadtree::nodeAddObject: trying to add an object already present in the node\n";
    else
    {
        node->objects.push_back(obj);
        _objectToNode[obj->id()] = node;
    }
}

void Quadtree::nodeRemoveObject(Node* node, Object* obj)
{
    auto it = std::find_if(std::begin(node->objects), std::end(node->objects),
        [this, &obj](const auto& rhs) { return obj->id() == rhs->id(); });
    
    if (it == std::end(node->objects) && VERBOSE)
        std::cerr << "Quadtree::nodeRemoveObject: trying to remove an object that is not present in the node\n";
    else
    {
        // swap with the last element and pop back
        *it = std::move(node->objects.back());
        node->objects.pop_back();

        _objectToNode.erase(obj->id());
    }
}

bool Quadtree::tryMerge(Node* node)
{
    if (!node)
        throw "Quadtree::tryMerge: invalid node pointer";

    if (isLeaf(node))
        throw "Quadtree::tryMerge: only interior nodes can be merged";

    auto objectsCount = node->objects.size();
    for (const auto& child : node->children)
    {
        if (!isLeaf(child))
            return false;
        objectsCount += child->objects.size();
    }
    if (objectsCount <= MAX_OBJECTS_PER_NODE)
    {
        node->objects.reserve(objectsCount);
        // merge the objects of all the children
        for (const auto& child : node->children)
        {
            for (const auto& obj : child->objects)
                nodeAddObject(node, obj);
        }
        // remove the children
        for (auto& child : node->children)
            child = nullptr;
        return true;
    }
    else
        return false;
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

void Quadtree::queryIntersections(Node* node, std::vector<std::pair<Object*, Object*>>& intersections) const
{
    // Find intersections between objects stored in this node
    // Make sure to not report the same intersection twice
    for (int i = 0; i < node->objects.size(); ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            if (node->objects[i]->rect().intersects(node->objects[j]->rect()))
                intersections.emplace_back(node->objects[i], node->objects[j]);
        }
    }
    if (!isLeaf(node))
    {
        // objects in this node can intersect objects in descendants
        for (const auto& child : node->children)
        {
            for (const auto& obj : node->objects)
                queryIntersectionsInDescendants(child, obj, intersections);
        }
        // find intersections in children
        for (const auto& child : node->children)
            queryIntersections(child, intersections);
    }
}

void Quadtree::queryIntersectionsInDescendants(Node* node, Object* obj, std::vector<std::pair<Object*, Object*>>& intersections) const
{
    // test against the objects stored in this node
    for (const auto& other : node->objects)
    {
        if (obj->rect().intersects(other->rect()))
            intersections.emplace_back(obj, other);
    }
    // test against objects stored into descendants of this node
    if (!isLeaf(node))
    {
        for (const auto& child : node->children)
            queryIntersectionsInDescendants(child, obj, intersections);
    }
}