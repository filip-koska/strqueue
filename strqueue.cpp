#include <string>
#include <unordered_map>
#include <deque>
#include <cassert>
#include <limits>

#include "strqueue.h"

#ifdef NDEBUG
static constexpr bool debug = false;
#else
static constexpr bool debug = true;
#endif


using std::cerr;
using std::unordered_map;
using std::deque;
using std::string;
using std::to_string;
using std::is_null_pointer;
using std::is_integral;
using std::is_same;
using std::numeric_limits;


namespace {

// prevents static initialisation order fiasco
unsigned long &get_cnt(void) {
    static unsigned long cnt = 0;
    return cnt;
}

// purpose same as above
unordered_map<unsigned long, deque<string>> &get_queues(void) {
    static unordered_map<unsigned long, deque<string>> queues 
                = unordered_map<unsigned long, deque<string>>();
    return queues;
}

// returns an empty queue for the sake of queue lexicographical comparison
deque<string> &get_empty_queue(void) {
    static deque<string> empty_queue = deque<string>();
    return empty_queue;
}

deque<string>::const_iterator deque_get_iterator_at(const deque<string> &queue,
                                                        size_t position) {
    size_t index = 0;
    auto it = queue.begin();

    while (index < position) {
        ++index;
        ++it;
    }

    return it;
}

// log information about the most recently called function
inline void debug_call(string name, string params) {
    cerr << name << "(" << params << ")\n";
}

// log information about the current function's return value
template<typename T>
inline void debug_return(string name, T value) {
    if constexpr (is_null_pointer<T>::value)
        cerr << name << " returns NULL\n";
    else if constexpr (is_integral<T>::value)
        cerr << name << " returns " << value << "\n";
    else if constexpr (is_same<T, const char*>::value)
        cerr << name << " returns \"" << value << "\"\n";
}

// log message that a void-type function execution ended
inline void debug_done(string name) {
    cerr << name << " done\n";
}

// log error information about non-existent queue in the map
inline void debug_doesnt_exist(string name, unsigned long id) {
    cerr << name << ": queue " << id << " does not exist\n";
}

// log error information about invalid string position in queue
inline void debug_doesnt_contain(string name, unsigned long id, size_t position) {
    cerr << name << ": queue " << id << " does not contain string at position " 
            << position << "\n";
}

// log information that a function couldn't fulfill its purpose
inline void debug_failed(string name) {
    cerr << name <<" failed\n";
}
} // namespace

namespace cxx {

unsigned long strqueue_new(void) {
    if constexpr (debug)
        debug_call(__func__, "");

    unsigned long &cnt = get_cnt();

    // check if there are valid IDs
    assert(cnt < numeric_limits<unsigned long>::max());

    auto &queues = get_queues();
    queues[cnt] = deque<string>();

    if constexpr (debug)
        debug_return(__func__, cnt);

    return cnt++;
}

void strqueue_delete(unsigned long id) {
    if constexpr (debug) 
        debug_call(__func__, to_string(id));

    auto &queues = get_queues();
    auto queue_it = queues.find(id);

    // queue does not exist
    if (queue_it == queues.end()) {
        if constexpr (debug)
            debug_doesnt_exist(__func__, id);
        
        return;
    }

    queues.erase(queue_it);

    if constexpr (debug)
        debug_done(__func__);
}

size_t strqueue_size(unsigned long id) {
    if constexpr (debug)
        debug_call(__func__, to_string(id));

    auto &queues = get_queues();
    auto queue_it = queues.find(id);

    // queue does not exist
    if (queue_it == queues.end()) {
        if constexpr (debug) {
            debug_doesnt_exist(__func__, id);
            debug_return(__func__, 0);
        }

        return 0;
    }

    const auto &queue = queue_it->second;

    if constexpr (debug)
        debug_return(__func__, queue.size());

    return queue.size();
}

void strqueue_insert_at(unsigned long id, size_t position, const char *str) {
    if constexpr (debug) {
        if (str == NULL)
            debug_call(__func__, to_string(id) + string(", ") 
                            + to_string(position) + string(", NULL"));
        else
            debug_call(__func__, to_string(id) + string(", ") 
                            + to_string(position) + string(", \"") 
                            + str + string("\""));

    }

    auto &queues = get_queues();
    auto queue_it = queues.find(id);
    string s;

    if (queue_it == queues.end() || str == NULL) {
        if constexpr (debug) {
            // queue does not exist
            if (queue_it == queues.end())
                debug_doesnt_exist(__func__, id);

            if(str == NULL)
                debug_failed(__func__);
        }

        return;
    }

    auto &queue = queue_it->second;

    s = str;

    if (queue.size() <= position)
        queue.push_back(s);
    else
        queue.insert(deque_get_iterator_at(queue, position), s);

    if constexpr (debug) 
        debug_done(__func__);
}

void strqueue_remove_at(unsigned long id, size_t position) {
    if constexpr (debug)
        debug_call(__func__, to_string(id) + string(", ") 
                        + to_string(position));

    auto &queues = get_queues();
    auto queue_it = queues.find(id);
    auto &queue = (queue_it == queues.end()) 
                    ? get_empty_queue() : queue_it->second;

    if (queue_it == queues.end() || queue.size() <= position) {
        if constexpr (debug) {
            // queue does not exist
            if (queue_it == queues.end())
                debug_doesnt_exist(__func__, id);
            // invalid element position
            else
                debug_doesnt_contain(__func__, id, position);
        }

        return;
    }

    queue.erase(deque_get_iterator_at(queue, position));

    if constexpr (debug)
        debug_done(__func__);
}

const char* strqueue_get_at(unsigned long id, size_t position) {
    if constexpr (debug)
        debug_call(__func__, to_string(id) + string(", ") 
                        + to_string(position));

    auto &queues = get_queues();
    auto queue_it = queues.find(id);
    const auto &queue = (queue_it == queues.end()) 
                            ? get_empty_queue() : queue_it->second;
    const char *res;

    if (queue_it == queues.end() || queue.size() <= position) {
        if constexpr (debug) {
            // queue does not exist
            if (queue_it == queues.end())
                debug_doesnt_exist(__func__, id);
            // invalid element position
            else
                debug_doesnt_contain(__func__, id, position);

            debug_return(__func__, nullptr);
        }

        return NULL;
    }

    res = deque_get_iterator_at(queue, position)->c_str();
    if constexpr (debug)
        debug_return(__func__, res);

    return res;
}

void strqueue_clear(unsigned long id) {
    if constexpr (debug)
        debug_call(__func__, to_string(id));

    auto &queues = get_queues();
    auto queue_it = queues.find(id);

    // queue does not exist
    if (queue_it == queues.end()) {
        if constexpr (debug)
            debug_doesnt_exist(__func__, id);
        return;
    }

    auto &queue = queue_it->second;

    queue.clear();

    if constexpr (debug)
        debug_done(__func__);
}

int strqueue_comp(unsigned long id1, unsigned long id2) {
    if constexpr (debug)
        debug_call(__func__, to_string(id1) + string(", ") + to_string(id2));

    const auto &queues = get_queues();
    const auto q1_it = queues.find(id1), q2_it = queues.find(id2);

    // check if any of the queues does not exist
    const bool not_in_1 = (q1_it == queues.end()), not_in_2 = (q2_it == queues.end());
    // if a queue does not exist, it is treated as an empty queue
    const auto &q1 = not_in_1 ? get_empty_queue() : q1_it->second;
    const auto &q2 = not_in_2 ? get_empty_queue() : q2_it->second;
    int res = 2;

    if constexpr (debug) {
        if(not_in_1)
            debug_doesnt_exist(__func__, id1);

        if(not_in_2)
            debug_doesnt_exist(__func__, id2);       
    }
    
    if (q1 < q2)
        res = -1;
    else if (q1 > q2)
        res = 1;
    else res = 0;

    if constexpr (debug)
        debug_return(__func__, res);   

    return res;
}
} // namespace cxx
