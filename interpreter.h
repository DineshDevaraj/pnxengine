
#pragma once

#include "rtree.h"
#include <iostream>

typedef std::string Str;
typedef std::deque<Str> Strdeq_t;
typedef Strdeq_t::iterator Strdeqitr_t;

Intdeq_t get_series(const RTree_t *nptr);

Strdeq_t repeat_rclass();
Strdeq_t arithprog_rclass();
Strdeq_t constant_rclass();

Strdeq_t rule_interpreter(const RTree_t *nptr);