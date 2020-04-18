#pragma once

#include <array>

namespace GibberishDetectorData {

constexpr size_t ACCEPTED_CHARS_COUNT = 27;
template<typename T, size_t N> using Matrix2D = std::array<std::array<T, N>, N>;
using MatrixCounts = Matrix2D<double, ACCEPTED_CHARS_COUNT>;

extern const std::array<char, ACCEPTED_CHARS_COUNT> ACCEPTED_CHARS;
extern const double TRAINED_THRESHOLD;
extern const MatrixCounts TRAINED_COUNTS;

} // namespace GibberishDetectorData
