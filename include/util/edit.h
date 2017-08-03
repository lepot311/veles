/*
 * Copyright 2017 CodiLime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#pragma once

#include <memory>
#include <stdlib.h>

#include <QMap>
#include <QVector>

#include "data/bindata.h"
#include "ui/fileblobmodel.h"

namespace veles {
namespace util {

class EditEngine {
 public:
  explicit EditEngine(ui::FileBlobModel* original_data,
                      int edit_stack_limit = 100)
      : original_data_(original_data),
        bindata_width_(original_data->binData().width()),
        edit_stack_limit_(edit_stack_limit) {
    initAddressMapping();
  }

  void changeBytes(size_t pos, const data::BinData& bytes,
                   bool add_to_history = true);

  bool hasUndo() const { return !edit_stack_.isEmpty(); }
  /** Undo last changeBytes and returns first byte changed by this operation */
  size_t undo();

  bool hasChanges() const { return !changes_.isEmpty(); }
  void applyChanges();
  void clear();

  uint64_t byteValue(size_t byte_pos) const;
  uint64_t originalByteValue(size_t byte_pos) const;
  data::BinData bytesValues(size_t offset, size_t size) const;
  data::BinData originalBytesValues(size_t offset, size_t size) const;

 private:
  struct EditNode {
    std::shared_ptr<data::BinData> fragment_;
    size_t offset_;

    explicit EditNode(const data::BinData& fragment, size_t offset)
        : fragment_(std::make_shared<data::BinData>(fragment)),
          offset_(offset) {}
    explicit EditNode(size_t offset) : fragment_(nullptr), offset_(offset) {}
  };

  ui::FileBlobModel* original_data_;
  uint32_t bindata_width_;

  QMap<size_t, EditNode> address_mapping_;
  QMap<size_t, data::BinData> changes_;

  int edit_stack_limit_;
  QList<data::BinData> edit_stack_data_;
  QList<QPair<size_t, size_t>> edit_stack_;

  void initAddressMapping();

  QMap<size_t, data::BinData>::const_iterator itFromPos(size_t pos) const;
  QMap<size_t, data::BinData> changesFromRange(size_t pos, size_t size) const;
  void applyChangesOnBinData(data::BinData* data, size_t offset = 0,
                             int64_t max_bytes = -1) const;
};

}  // namespace util
}  // namespace veles
