#ifndef __CSV_WORK_ORDER_CSV_WORK_ORDER_ITERATOR_INTERFACE_H__
#define __CSV_WORK_ORDER_CSV_WORK_ORDER_ITERATOR_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <interface/iterator/iterator_interface.h>
#include <model/csv_work_order.h>

namespace csv_work_order
{
    class CsvWorkOrderIteratorInterface: public virtual iterator::IteratorInterface<model::CSVWorkOrder>{};
}

#endif