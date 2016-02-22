#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct MatcherOptions {
  bool case_sensitive;
  size_t num_threads;
  size_t max_results;
};

struct MatchResult {
  double score;
  // We can't afford to copy strings around while we're ranking them.
  // These are not guaranteed to last very long and should be copied out ASAP.
  const char* value;

  // Order small scores to the top of any priority queue.
  // We need a min-heap to maintain the top-N results.
  bool operator<(const MatchResult& other) const {
    return score > other.score;
  }
};

class MatcherBase {
public:
  struct CandidateData {
    std::string lowercase;
    /**
     * A bitmask of the letters (a-z) contained in the string.
     * ('a' = 1, 'b' = 2, 'c' = 4, ...)
     * We can then compute the bitmask of the query and very quickly prune out
     * non-matches in many practical cases.
     */
    int bitmask;
  };

  std::vector<MatchResult> findMatches(const std::string &query,
                                       const MatcherOptions &options);
  void addCandidate(const std::string &candidate);
  void removeCandidate(const std::string &candidate);
  void clear();
  void reserve(size_t n);
  size_t size();

private:
  std::unordered_map<std::string, CandidateData> candidates_;
};
