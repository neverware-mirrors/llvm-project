//===- MappedBlockStream.h - Discontiguous stream data in an Msf -*- C++
//-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_DEBUGINFO_MSF_MAPPEDBLOCKSTREAM_H
#define LLVM_DEBUGINFO_MSF_MAPPEDBLOCKSTREAM_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/DebugInfo/Msf/MsfStreamLayout.h"
#include "llvm/DebugInfo/Msf/StreamInterface.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/Error.h"
#include <cstdint>
#include <vector>

namespace llvm {
namespace msf {

struct MsfLayout;

/// MappedBlockStream represents data stored in an Msf file into chunks of a
/// particular size (called the Block Size), and whose chunks may not be
/// necessarily contiguous.  The arrangement of these chunks within the file
/// is described by some other metadata contained within the Msf file.  In
/// the case of a standard Msf Stream, the layout of the stream's blocks
/// is described by the Msf "directory", but in the case of the directory
/// itself, the layout is described by an array at a fixed location within
/// the Msf.  MappedBlockStream provides methods for reading from and writing
/// to one of these streams transparently, as if it were a contiguous sequence
/// of bytes.
class MappedBlockStream : public ReadableStream {
  friend class WritableMappedBlockStream;

public:
  static std::unique_ptr<MappedBlockStream>
  createStream(uint32_t BlockSize, uint32_t NumBlocks,
               const MsfStreamLayout &Layout, const ReadableStream &MsfData);

  static std::unique_ptr<MappedBlockStream>
  createIndexedStream(const MsfLayout &Layout, const ReadableStream &MsfData,
                      uint32_t StreamIndex);

  static std::unique_ptr<MappedBlockStream>
  createDirectoryStream(const MsfLayout &Layout, const ReadableStream &MsfData);

  Error readBytes(uint32_t Offset, uint32_t Size,
                  ArrayRef<uint8_t> &Buffer) const override;
  Error readLongestContiguousChunk(uint32_t Offset,
                                   ArrayRef<uint8_t> &Buffer) const override;

  uint32_t getLength() const override;

  uint32_t getNumBytesCopied() const;

  llvm::BumpPtrAllocator &getAllocator() { return Pool; }

  void invalidateCache();

  uint32_t getBlockSize() const { return BlockSize; }
  uint32_t getNumBlocks() const { return NumBlocks; }
  uint32_t getStreamLength() const { return StreamLayout.Length; }

protected:
  MappedBlockStream(uint32_t BlockSize, uint32_t NumBlocks,
                    const MsfStreamLayout &StreamLayout,
                    const ReadableStream &MsfData);

private:
  const MsfStreamLayout &getStreamLayout() const { return StreamLayout; }
  void fixCacheAfterWrite(uint32_t Offset, ArrayRef<uint8_t> Data) const;

  Error readBytes(uint32_t Offset, MutableArrayRef<uint8_t> Buffer) const;
  bool tryReadContiguously(uint32_t Offset, uint32_t Size,
                           ArrayRef<uint8_t> &Buffer) const;

  const uint32_t BlockSize;
  const uint32_t NumBlocks;
  const MsfStreamLayout StreamLayout;
  const ReadableStream &MsfData;

  typedef MutableArrayRef<uint8_t> CacheEntry;
  mutable llvm::BumpPtrAllocator Pool;
  mutable DenseMap<uint32_t, std::vector<CacheEntry>> CacheMap;
};

class WritableMappedBlockStream : public WritableStream {
public:
  static std::unique_ptr<WritableMappedBlockStream>
  createStream(uint32_t BlockSize, uint32_t NumBlocks,
               const MsfStreamLayout &Layout, const WritableStream &MsfData);

  static std::unique_ptr<WritableMappedBlockStream>
  createIndexedStream(const MsfLayout &Layout, const WritableStream &MsfData,
                      uint32_t StreamIndex);

  static std::unique_ptr<WritableMappedBlockStream>
  createDirectoryStream(const MsfLayout &Layout, const WritableStream &MsfData);

  Error readBytes(uint32_t Offset, uint32_t Size,
                  ArrayRef<uint8_t> &Buffer) const override;
  Error readLongestContiguousChunk(uint32_t Offset,
                                   ArrayRef<uint8_t> &Buffer) const override;
  uint32_t getLength() const override;

  Error writeBytes(uint32_t Offset, ArrayRef<uint8_t> Buffer) const override;

  Error commit() const override;

  const MsfStreamLayout &getStreamLayout() const {
    return ReadInterface.getStreamLayout();
  }
  uint32_t getBlockSize() const { return ReadInterface.getBlockSize(); }
  uint32_t getNumBlocks() const { return ReadInterface.getNumBlocks(); }
  uint32_t getStreamLength() const { return ReadInterface.getStreamLength(); }

protected:
  WritableMappedBlockStream(uint32_t BlockSize, uint32_t NumBlocks,
                            const MsfStreamLayout &StreamLayout,
                            const WritableStream &MsfData);

private:
  MappedBlockStream ReadInterface;

  const WritableStream &WriteInterface;
};

} // end namespace pdb
} // end namespace llvm

#endif // LLVM_DEBUGINFO_MSF_MAPPEDBLOCKSTREAM_H
