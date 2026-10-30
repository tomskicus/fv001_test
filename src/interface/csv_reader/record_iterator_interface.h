#ifndef __CSV_READER_RECORD_ITERATOR_INTERFACE_H__
#define __CSV_READER_RECORD_ITERATOR_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <optional>
#include <string>
#include <interface/iterator/iterator_interface.h>

namespace csv_reader
{
    class RecordIteratorInterface: public virtual iterator::IteratorInterface<std::string>{};
}

#endif