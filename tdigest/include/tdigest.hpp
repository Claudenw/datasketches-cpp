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

#ifndef _TDIGEST_HPP_
#define _TDIGEST_HPP_

#include <type_traits>
#include <limits>

#include "common_defs.hpp"

namespace datasketches {

// this is equivalent of K_2 (default) in the Java implementation mentioned below
// Generates cluster sizes proportional to q*(1-q).
// The use of a normalizing function results in a strictly bounded number of clusters no matter how many samples.
struct scale_function {
  double k(double q, double normalizer) const {
    return limit([normalizer] (double q) { return std::log(q / (1 - q)) * normalizer; }, q, 1e-15, 1 - 1e-15);
  }
  double q(double k, double normalizer) const {
    const double w = std::exp(k / normalizer);
    return w / (1 + w);
  }
  double max(double q, double normalizer) const {
    return q * (1 - q) / normalizer;
  }
  double normalizer(double compression, double n) const {
    return compression / z(compression, n);
  }
  double z(double compression, double n) const {
    return 4 * std::log(n / compression) + 24;
  }

  template<typename Func>
  double limit(Func f, double x, double low, double high) const {
    if (x < low) return f(low);
    if (x > high) return f(high);
    return f(x);
  }
};

// forward declaration
template <typename T, typename Allocator = std::allocator<T>> class tdigest;

/// TDigest float sketch
using tdigest_float = tdigest<float>;
/// TDigest double sketch
using tdigest_double = tdigest<double>;

/**
 * t-Digest for estimating quantiles and ranks.
 * This implementation is based on the following paper:
 * Ted Dunning, Otmar Ertl. Extremely Accurate Quantiles Using t-Digests
 * and the following implementation in Java:
 * https://github.com/tdunning/t-digest
 * This implementation is similar to MergingDigest in the above Java implementation
 */
template <typename T, typename Allocator>
class tdigest {
  static_assert(std::is_floating_point<T>::value, "Floating-point type expected");
  static_assert(std::numeric_limits<T>::is_iec559, "IEEE 754 compatibility required");
public:
  using value_type = T;
  using allocator_type = Allocator;

  static const bool USE_ALTERNATING_SORT = true;
  static const bool USE_TWO_LEVEL_COMPRESSION = true;
  static const bool USE_WEIGHT_LIMIT = true;

  class centroid {
  public:
    centroid(T value, uint64_t weight): mean_(value), weight_(weight) {}
    void add(const centroid& other) {
      weight_ += other.weight_;
      mean_ += (other.mean_ - mean_) * other.weight_ / weight_;
    }
    T get_mean() const { return mean_; }
    uint64_t get_weight() const { return weight_; }
  private:
    T mean_;
    uint64_t weight_;
  };
  using vector_centroid = std::vector<centroid, typename std::allocator_traits<Allocator>::template rebind_alloc<centroid>>;
  using vector_bytes = std::vector<uint8_t, typename std::allocator_traits<Allocator>::template rebind_alloc<uint8_t>>;

  struct centroid_cmp {
    centroid_cmp() {}
    bool operator()(const centroid& a, const centroid& b) const {
      if (a.get_mean() < b.get_mean()) return true;
      return false;
    }
  };

  /**
   * Constructor
   * @param k affects the size of the sketch and its estimation error
   * @param allocator used to allocate memory
   */
  explicit tdigest(uint16_t k = 100, const Allocator& allocator = Allocator());

  /**
   * Update this t-Digest with the given value
   * @param value to update the t-Digest with
   */
  void update(T value);

  /**
   * Merge the given t-Digest into this one
   * @param other t-Digest to merge
   */
  void merge(tdigest& other);

  /**
   * Process buffered values and merge centroids if needed
   */
  void compress();

  /**
   * @return true if t-Digest has not seen any data
   */
  bool is_empty() const;

  /**
   * @return minimum value seen by t-Digest
   */
  T get_min_value() const;

  /**
   * @return maximum value seen by t-Digest
   */
  T get_max_value() const;

  /**
   * @return total weight
   */
  uint64_t get_total_weight() const;

  /**
   * Compute approximate normalized rank of the given value.
   * @param value to be ranked
   * @return normalized rank (from 0 to 1 inclusive)
   */
  double get_rank(T value) const;

  /**
   * Compute approximate quantile value corresponding to the given normalized rank
   * @param rank normalized rank (from 0 to 1 inclusive)
   * @return quantile value corresponding to the given rank
   */
  T get_quantile(double rank) const;

  /**
   * @return parameter k (compression) that was used to configure this t-Digest
   */
  uint16_t get_k() const;

  /**
   * Human-readable summary of this t-Digest as a string
   * @param print_centroids if true append the list of centroids with weights
   * @return summary of this t-Digest
   */
  string<Allocator> to_string(bool print_centroids = false) const;

  /**
   * This method serializes t-Digest into a given stream in a binary form
   * @param os output stream
   */
  void serialize(std::ostream& os) const;

  /**
   * This method serializes t-Digest as a vector of bytes.
   * An optional header can be reserved in front of the sketch.
   * It is an uninitialized space of a given size.
   * @param header_size_bytes space to reserve in front of the sketch
   * @return serialized sketch as a vector of bytes
   */
  vector_bytes serialize(unsigned header_size_bytes = 0) const;

  /**
   * This method deserializes t-Digest from a given stream.
   * @param is input stream
   * @param allocator instance of an Allocator
   * @return an instance of t-Digest
   */
  static tdigest deserialize(std::istream& is, const Allocator& allocator = Allocator());

  /**
   * This method deserializes t-Digest from a given array of bytes.
   * @param bytes pointer to the array of bytes
   * @param size the size of the array
   * @param allocator instance of an Allocator
   * @return an instance of t-Digest
   */
  static tdigest deserialize(const void* bytes, size_t size, const Allocator& allocator = Allocator());

private:
  Allocator allocator_;
  bool reverse_merge_;
  uint16_t k_;
  uint16_t internal_k_;
  T min_;
  T max_;
  size_t centroids_capacity_;
  vector_centroid centroids_;
  uint64_t centroids_weight_;
  size_t buffer_capacity_;
  vector_centroid buffer_;
  uint64_t buffered_weight_;

  static const uint8_t PREAMBLE_LONGS_EMPTY = 1;
  static const uint8_t PREAMBLE_LONGS_NON_EMPTY = 2;
  static const uint8_t SERIAL_VERSION = 1;
  static const uint8_t SKETCH_TYPE = 20;

  static const uint8_t COMPAT_DOUBLE = 1;
  static const uint8_t COMPAT_FLOAT = 2;

  enum flags { IS_EMPTY, REVERSE_MERGE };

  // for deserialize
  tdigest(bool reverse_merge, uint16_t k, T min, T max, vector_centroid&& centroids, uint64_t total_weight_, const Allocator& allocator);

  void merge_buffered();

  static double weighted_average(double x1, double w1, double x2, double w2);

  // for compatibility with format of the reference implementation
  static tdigest deserialize_compat(std::istream& is, const Allocator& allocator = Allocator());
  static tdigest deserialize_compat(const void* bytes, size_t size, const Allocator& allocator = Allocator());
};

} /* namespace datasketches */

#include "tdigest_impl.hpp"

#endif // _TDIGEST_HPP_
