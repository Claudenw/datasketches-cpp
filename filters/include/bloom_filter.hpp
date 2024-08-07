/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _BLOOM_FILTER_HPP_
#define _BLOOM_FILTER_HPP_

#include <cstdint>
#include <memory>
#include <vector>

#include "common_defs.hpp"

namespace datasketches {

// forward declarations
template<typename A> class bloom_filter_alloc;
template<typename A> class bloom_filter_builder_alloc;

// aliases with default allocator
using bloom_filter = bloom_filter_alloc<std::allocator<uint8_t>>;
using bloom_filter_builder = bloom_filter_builder_alloc<std::allocator<uint8_t>>;

template<typename Allocator = std::allocator<uint8_t>>
class bloom_filter_builder_alloc {
  using A = Allocator;

public:
  static uint16_t suggest_num_hashes(const uint64_t num_distinct_items, const uint64_t num_filter_bits);
  static uint16_t suggest_num_hashes(const double target_false_positive_prob);
  static uint64_t suggest_num_filter_bits(const uint64_t num_distinct_items, const double target_false_positive_prob);

  static bloom_filter_alloc<A> create_by_accuracy(const uint64_t num_distinct_items,
                                                  const double target_false_positive_prob,
                                                  const Allocator& allocator = Allocator());
  static bloom_filter_alloc<A> create_by_accuracy(const uint64_t num_distinct_items,
                                                  const double target_false_positive_prob,
                                                  const uint64_t seed,
                                                  const Allocator& allocator = Allocator());

  static bloom_filter_alloc<A> create_by_size(const uint64_t num_bits,
                                              const uint16_t num_hashes,
                                              const Allocator& allocator = Allocator());
  static bloom_filter_alloc<A> create_by_size(const uint64_t num_bits,
                                              const uint16_t num_hashes,
                                              const uint64_t seed,
                                              const Allocator& allocator = Allocator());

};

template<typename Allocator = std::allocator<uint8_t>>
class bloom_filter_alloc {
  using A = Allocator;

public:

  /**
   * Checks if the Bloom Filter has processed any items
   * @return True if the BloomFilter is empty, otherwise False
   */
  bool is_empty() const;

  /**
   * Returns the number of bits in the Bloom Filter that are set to 1.
   * @return The number of bits in use in this filter
   */
  uint64_t get_bits_used();

  /**
   * Returns the total number of bits in the Bloom Filter.
   * @return The total size of the Bloom Filter
   */
  uint64_t get_capacity() const;

  /**
   * Returns the configured number of hash functions for this Bloom Filter
   * @return The number of hash functions to apply to inputs
   */
  uint16_t get_num_hashes() const;

  /**
   * Returns the hash seed for this Bloom Filter.
   * @return The hash seed for this filter
   */
  uint64_t get_seed() const;

  /**
   * Resets the Bloom Filter to its original state.
   */
  void reset();

  // UPDATE METHODS

  /**
   * Updates the filter with the given std::string.
   * The string is converted to a byte array using UTF8 encoding.
   * If the string is null or empty no update attempt is made and the method returns.
   * @param item The given string.
   */
  void update(const std::string& item);

  /**
   * Updates the filter with the given unsigned 64-bit integer.
   * @param item The given integer.
   */
  void update(uint64_t item);

  /**
   * Updates the filter with the given unsigned 32-bit integer.
   * @param item The given integer.
   */
  void update(uint32_t item);

  /**
   * Updates the filter with the given unsigned 16-bit integer.
   * @param item The given integer.
   */
  void update(uint16_t item);

  /**
   * Updates the filter with the given unsigned 8-bit integer.
   * @param item The given integer.
   */
  void update(uint8_t item);

  /**
   * Updates the filter with the given signed 64-bit integer.
   * @param item The given integer.
   */
  void update(int64_t item);

  /**
   * Updates the filter with the given signed 32-bit integer.
   * @param item The given integer.
   */
  void update(int32_t item);

  /**
   * Updates the filter with the given signed 16-bit integer.
   * @param item The given integer.
   */
  void update(int16_t item);

  /**
   * Updates the filter with the given signed 8-bit integer.
   * @param item The given integer.
   */
  void update(int8_t item);

  /**
   * Updates the filter with the given 64-bit floating point value.
   * @param item The given double.
   */
  void update(double item);

  /**
   * Updates the filter with the give 32-bit floating point value.
   * @param item The given float.
   */
  void update(float item);

  /**
   * Updates the filter with the given data array.
   * @param data The given array.
   * @param length_bytes The array length in bytes.
   */
  void update(const void* data, size_t length_bytes);

  // QUERY-AND-UPDATE METHODS

  /**
   * Updates the filter with the given std::string and returns the result from
   * querying the filter prior to the update.
   * The string is converted to a byte array using UTF8 encoding.
   * If the string is null or empty no update attempt is made and the method returns false.
   * @param item The given string.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(const std::string& item);

  /**
   * Updates the filter with the given unsigned 64-bit integer and returns the result from
   * querying the filter prior to the update.
   * @param item The given integer.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(uint64_t item);

  /**
   * Updates the filter with the given unsigned 32-bit integer and returns the result from
   * querying the filter prior to the update.
   * @param item The given integer.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(uint32_t item);

  /**
   * Updates the filter with the given unsigned 16-bit integer and returns the result from
   * querying the filter prior to the update.
   * @param item The given integer.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(uint16_t item);

  /**
   * Updates the filter with the given unsigned 8-bit integer and returns the result from
   * querying the filter prior to the update.
   * @param item The given integer.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(uint8_t item);

  /**
   * Updates the filter with the given signed 64-bit integer and returns the result from
   * querying the filter prior to the update.
   * @param item The given integer.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(int64_t item);

  /**
   * Updates the filter with the given signed 32-bit integer and returns the result from
   * querying the filter prior to the update.
   * @param item The given integer.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(int32_t item);

  /**
   * Updates the filter with the given signed 16-bit integer and returns the result from
   * querying the filter prior to the update.
   * @param item The given integer.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(int16_t item);

  /**
   * Updates the filter with the given signed 8-bit integer and returns the result from
   * querying the filter prior to the update.
   * @param item The given integer.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(int8_t item);

  /**
   * Updates the filter with the given 64-bit floating point value and returns the result from
   * querying the filter prior to the update.
   * @param item The given double.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(double item);

  /**
   * Updates the filter with the give 32-bit floating point value and returns the result from
   * querying the filter prior to the update.
   * @param item The given float.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(float item);

  /**
   * Updates the filter with the given data array and returns the result from
   * querying the filter prior to the update.
   * @param data The given array.
   * @param length_bytes The array length in bytes.
   * @return The result from querying the filter prior to the update.
   */
  bool query_and_update(const void* data, size_t length_bytes);

  // QUERY METHODS

  /**
   * Queries the filter with the given std::string and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * The string is converted to a byte array using UTF8 encoding.
   * If the string is null or empty the method always returns false.
   * @param item The given string.
   * @return The result from querying the filter with the given item.
   */
  bool query(const std::string& item) const;

  /**
   * Queries the filter with the given unsigned 64-bit integer and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param item The given integer.
   * @return The result from querying the filter with the given item.
   */
  bool query(uint64_t item) const;

  /**
   * Queries the filter with the given unsigned 32-bit integer and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param item The given integer.
   * @return The result from querying the filter with the given item.
   */
  bool query(uint32_t item) const;

  /**
   * Queries the filter with the given unsigned 16-bit integer and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param item The given integer.
   * @return The result from querying the filter with the given item.
   */
  bool query(uint16_t item) const;

  /**
   * Queries the filter with the given unsigned 8-bit integer and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param item The given integer.
   * @return The result from querying the filter with the given item.
   */
  bool query(uint8_t item) const;

  /**
   * Queries the filter with the given signed 64-bit integer and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param item The given integer.
   * @return The result from querying the filter with the given item.
   */
  bool query(int64_t item) const;

  /**
   * Queries the filter with the given signed 32-bit integer and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param item The given integer.
   * @return The result from querying the filter with the given item.
   */
  bool query(int32_t item) const;

  /**
   * Queries the filter with the given signed 16-bit integer and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param item The given integer.
   * @return The result from querying the filter with the given item.
   */
  bool query(int16_t item) const;

  /**
   * Queries the filter with the given signed 8-bit integer and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param item The given integer.
   * @return The result from querying the filter with the given item.
   */
  bool query(int8_t item) const;

  /**
   * Queries the filter with the given 64-bit floating point value and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param item The given double.
   * @return The result from querying the filter with the given item.
   */
  bool query(double item) const;

  /**
   * Queries the filter with the given 32-bit floating point value and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param item The given float.
   * @return The result from querying the filter with the given item.
   */
  bool query(float item) const;

  /**
   * Queries the filter with the given data array. and returns the result from
   * Queries the filter with the given 64-bit floating point value and returns whether the value
   * might have been seen previoiusly. The filter's expected Fale Positive Probability
   * determines the chances of a true result being a false positive. False engatives are
   * never possible.
   * @param data The given array.
   * @param length_bytes The array length in bytes.
   * @return The result from querying the filter with the given item.
   */
  bool query(const void* data, size_t length_bytes) const;

  // OTHER OPERATIONS

  /**
   * Unions two Bloom Filters by applying a logical OR. The result will recognized
   * any values seen by either filter (as well as false positives).
   * @param other A BloomFilter to union with this one
   */
  void union_with(const bloom_filter_alloc& other);

  /**
   * Intersects two Bloom Filters by applying a logical AND. The result will recognize
   * only values seen by both filters (as well as false positives).
   * @param other A Bloom Filter to union with this one
   */
  void intersect(const bloom_filter_alloc& other);

  /**
   * Inverts all the bits of the BloomFilter. Approximately inverts the notion of set-membership.
   */
  void invert();

  /**
   * Helps identify if two Bloom Filters may be unioned or intersected.
   * @param other A Bloom Filter to check for compatibility with this one
   * @return True if the filters are compatible, otherwise false
   */
   bool is_compatible(const bloom_filter_alloc& other) const;

  // TODO: Serialization

  /**
   * @brief Returns a human-readable string representation of the Bloom Filter.
   * @param print_filter If true, the filter bits will be printed as well.
   * @return A human-readable string representation of the Bloom Filter.
   */
  string<A> to_string(bool print_filter = false) const;

  /**
   * @brief Destroy the bloom filter alloc object
   */
  ~bloom_filter_alloc();

private:
  static const uint64_t MAX_HEADER_SIZE_BYTES = 32; // 4 Java Longs
  static const uint64_t MAX_FILTER_SIZE_BITS = (INT32_MAX - MAX_HEADER_SIZE_BYTES) * sizeof(uint64_t);

  bloom_filter_alloc(const uint64_t num_bits, const uint16_t num_hashes, const uint64_t seed, const Allocator& allocator = Allocator());

  // internal query/update methods
  void internal_update(const uint64_t h0, const uint64_t h1);
  bool internal_query_and_update(const uint64_t h0, const uint64_t h1);
  bool internal_query(const uint64_t h0, const uint64_t h1) const;

  A allocator_;
  uint64_t seed_;
  uint16_t num_hashes_;
  bool is_dirty_;
  bool is_owned_; // if true, data is not owned by filter AND data_ holdes the entire filter not just the bit array
  bool is_read_only_; // if true, filter is read-only
  uint64_t capacity_bits_;
  uint64_t num_bits_set_;
  uint8_t* bit_array_;  // data backing bit_array_, regardless of ownership
  uint8_t* memory_; // if wrapped, pointer to the start of the filter, otheriwse nullptr

  friend class bloom_filter_builder_alloc<A>;
};

} // namespace datasketches

#include "bloom_filter_builder_impl.hpp"
#include "bloom_filter_impl.hpp"

#endif // _BLOOM_FILTER_HPP_ b
