#ifndef __CSV_WORK_ORDER_CSV_WORK_ORDER_AS_RECORD_ITERATOR_FACTORY_INTERFACE_H__
#define __CSV_WORK_ORDER_CSV_WORK_ORDER_AS_RECORD_ITERATOR_FACTORY_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <model/csv_work_order.h>
#include <interface/csv_reader/record_iterator_interface.h>
#include <memory>

namespace csv_work_order
{
    class CsvWorkOrderAsRecordIteratorFactoryInterface
    {
        public:

            virtual ~CsvWorkOrderAsRecordIteratorFactoryInterface() noexcept = default;

            virtual auto to_record_iterator(const model::CSVWorkOrder& csv_work_order) -> std::unique_ptr<csv_reader::RecordIteratorInterface> = 0;
    };
}

#endif