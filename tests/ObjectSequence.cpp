#include "Object.h"
#include "Scene.h"
#include <limits>
#include <stdexcept>
#include <vector>

using namespace agp;

static void check(bool condition, int line)
{
    if (!condition)
        throw std::runtime_error("Object sequence regression at line " + std::to_string(line));
}

#define require(condition) check((condition), __LINE__)

int main()
{
    Scene scene(RectF(0, 0, 100, 100), Point(16, 16));
    Object* object = new Object(&scene, RectF(0, 0, 1, 1));
    std::vector<int> log;
    object->sequence("intro", {
        {0, [&] { log.push_back(0); }},
        {2, [&] { log.push_back(1); }},
        {1, [&] { log.push_back(2); }}
    });
    require(log.empty());
    object->update(0);
    require(log == std::vector<int>{0});
    object->update(1);
    require(log == std::vector<int>{0});
    object->update(1);
    require(log == std::vector<int>({0, 1}));
    object->update(1);
    require(log == std::vector<int>({0, 1, 2}));

    log.clear();
    std::vector<std::pair<float, std::function<void()>>> steps;
    for (int i = 0; i < 12; ++i)
        steps.push_back({i % 2 ? 0.0f : 0.25f, [&, i] { log.push_back(i); }});
    object->sequence("many", steps);
    object->update(10); // Multiple deadlines in one update, including indices 10 and 11.
    for (int i = 0; i < 12; ++i)
        require(log.at(i) == i);
    require(log.size() == 12);

    log.clear();
    object->sequence("replace", {{1, [&] { log.push_back(1); }}, {1, [&] { log.push_back(2); }}});
    object->sequence("replace", {{1, [&] { log.push_back(3); }}});
    object->update(3);
    require(log == std::vector<int>{3});
    object->sequence("cancel", {{0, [&] { object->unscheduleSequence("cancel"); }},
                                {0, [&] { log.push_back(4); }}});
    object->update(1);
    require(log == std::vector<int>{3});

    object->sequence("restart", {{0, [&] {
        object->sequence("restart", {{0, [&] { log.push_back(5); }}});
    }}, {0, [&] { log.push_back(6); }}});
    object->update(1);
    require(log == std::vector<int>{3});
    object->update(0);
    require(log == std::vector<int>({3, 5}));

    object->sequence("valid", {{1, [&] { log.push_back(7); }}});
    for (float invalid : {-1.0f, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()})
    {
        bool rejected = false;
        try { object->sequence("valid", {{invalid, [] {}}}); }
        catch (const std::invalid_argument&) { rejected = true; }
        require(rejected);
    }
    bool emptyRejected = false;
    try { object->sequence("valid", {{0, {}}}); }
    catch (const std::invalid_argument&) { emptyRejected = true; }
    require(emptyRejected);
    bool overflowRejected = false;
    try { object->sequence("valid", {{std::numeric_limits<float>::max(), [] {}},
                                     {std::numeric_limits<float>::max(), [] {}}}); }
    catch (const std::invalid_argument&) { overflowRejected = true; }
    require(overflowRejected);
    object->update(1);
    require(log.back() == 7);
    object->sequence("empty", {{0, [&] { log.push_back(8); }}});
    object->sequence("empty", {});
    object->update(0);
    require(log.back() == 7);

    int repeats = 0;
    object->schedule("mutable", 1, [count = 0, &repeats]() mutable { repeats = ++count; }, 2);
    for (int i = 0; i < 4; ++i) object->update(1);
    require(repeats == 3);
    object->schedule("self", 0, [&] { object->unschedule("self"); }, -1);
    object->update(0);

    object->sequence("death", {{0, [&] { object->kill(); }}, {0, [&] { log.push_back(9); }}});
    object->update(0);
    require(log.back() == 7);
    scene.refreshObjects(); // Pending actions disappear with the object.

    Scheduler timer(0, [&] { ++repeats; });
    timer.update(0);
    timer.update(1);
    require(repeats == 4);
    require(!timer.on());
    int sceneRepeats = 0;
    scene.schedule("mutable", 1, [count = 0, &sceneRepeats]() mutable { sceneRepeats = ++count; }, 2);
    for (int i = 0; i < 4; ++i) scene.update(1);
    require(sceneRepeats == 3);
    scene.schedule("replace", 0, [&] { scene.schedule("replace", 1, [&] { ++sceneRepeats; }); });
    scene.update(0);
    require(sceneRepeats == 3);
    scene.update(1);
    require(sceneRepeats == 4);
    return 0;
}
