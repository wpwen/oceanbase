/**
 * (C) 2010-2012 Alibaba Group Holding Limited.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * Version: $Id$
 *
 * ob_dual_table_scan.h
 *
 * Authors:
 *   Guibin Du<tianguan.dgb@taobao.com>
 *
 */
#ifndef _OB_DUAL_TABLE_SCAN_H
#define _OB_DUAL_TABLE_SCAN_H
#include "sql/ob_no_children_phy_operator.h"
#include "common/utility.h"

namespace oceanbase
{
  namespace sql
  {
    class ObDualTableScan: public ObNoChildrenPhyOperator
    {
      public:
        ObDualTableScan() : is_opend_(false), has_data_(false)
        {
        }
        virtual ~ObDualTableScan() {}

        /// execute the insert statement
        virtual int open();
        virtual int close();
        virtual int64_t to_string(char* buf, const int64_t buf_len) const;

        /// @note first time return NULL value, second return OB_ITER_END
        virtual int get_next_row(const common::ObRow *&row);
        /// @note always return one cell (common::OB_INVALID_ID, common::OB_INVALID_ID)
        virtual int get_row_desc(const common::ObRowDesc *&row_desc) const;
      private:
        // disallow copy
        ObDualTableScan(const ObDropTable &other);
        ObDualTableScan& operator=(const ObDualTableScan &other);
        // function members
      private:
        // data members
        bool is_opend_;
        bool has_data_;
        ObRow row_;
        ObRowDesc row_desc_;
    };

    inline int ObDualTableScan::open() 
    {
      int ret = common::OB_SUCCESS;
      is_opend_ = true;
      has_data_ = true;
      if ((ret = row_desc_.add_column_desc(common::OB_INVALID_ID, common::OB_INVALID_ID)) 
        != common::OB_SUCCESS)
      {
        TBSYS_LOG(ERROR, "Construct row descriptor failed, ret=%d", ret);
      }
      else
      {
        row_.set_row_desc(row_desc_);
      }
      return ret;
    }

    inline int ObDualTableScan::close()
    {
      is_opend_ = false;
      has_data_ = false;
      row_.reset(false, ObRow::DEFAULT_NULL);
      row_desc_.reset();
      return common::OB_SUCCESS;
    }
    
    inline int ObDualTableScan::get_next_row(const common::ObRow *&row)
    {
      int ret = common::OB_SUCCESS;
      row = NULL;
      if (OB_UNLIKELY(!is_opend_))
      {
        ret = common::OB_NOT_INIT;
        TBSYS_LOG(ERROR, "Operator is not opened, ret=%d", ret);
      }
      else if (has_data_)
      {
        ObObj val;
        has_data_ = false;
        if ((ret = row_.set_cell(common::OB_INVALID_ID, common::OB_INVALID_ID, val))
          != common::OB_SUCCESS)
        {
          TBSYS_LOG(ERROR, "Get row failed, ret=%d", ret);
        }
        else
        {
          row = &row_;
        }
      }
      else
      {
        ret = common::OB_ITER_END;
      }
      return ret;
    }

    inline int ObDualTableScan::get_row_desc(const common::ObRowDesc *&row_desc) const
    {
      row_desc = &row_desc_;
      return common::OB_NOT_SUPPORTED;
    }

    inline int64_t ObDualTableScan::to_string(char* buf, const int64_t buf_len) const
    {
      int64_t pos = 0;
      databuff_printf(buf, buf_len, pos, "DualTableScan()\n");
      return pos;
    }
  } // end namespace sql
} // end namespace oceanbase

#endif /* _OB_DUAL_TABLE_SCAN_H */

