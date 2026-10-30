#ifndef __SEGMENT_ITERATOR_INTERFACE_H__
#define __SEGMENT_ITERATOR_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <optional>
#include <memory>
#include <interface/iterator/iterator_interface.h>

namespace csv_planner
{
    class SegmentIteratorInterface: public virtual iterator::IteratorInterface<std::pair<size_t, size_t>>{};
}

#endif