#pragma once

#include <string>

namespace komb {

const int kMilliFromUnit =       1000;
const int kMicroFromUnit =    1000000;
const int kNanoFromUnit  = 1000000000;

const float kUnitFromMilliFloat = 1e-3f;
const float kUnitFromMicroFloat = 1e-6f;
const float kUnitFromNanoFloat  = 1e-9f;

const double kUnitFromMilliDouble = 1e-3;
const double kUnitFromMicroDouble = 1e-6;
const double kUnitFromNanoDouble  = 1e-9;

const int kMillisecondsFromSeconds  = 1000;
const int kMillimetersFromMeters    = 1000;
const int k100MicrometersFromMeters = 10000;

const double kMetersFromMillimeters        = 1e-3;
const float  kMetersFromMillimetersFloat   = 1e-3f;
const double kMetersFrom100Micrometers     = 1e-4; // Aka dmm
const float  kSecondsFromMillisecondsFloat = 1e-3f;

/// Format with SI prefixes. sizeWithSiPrefix(3140000) == "3.14 G", sizeWithSiPrefix(42) == "42 "
/// Generally the result will be no more than 8 characters wide, e.g. "123.56 G"
std::string numberWithSiPrefix(double num);
std::string sizeWithSiPrefix(size_t num);

/**
 * The expected format is (number) [prefix] [unit] with any number of spaces anywhere.
 * The prefix, if any, is either a SI prefix (k, M, G, T, ...) or binary prefix (Ki, Mi, Gi, ...).
 * The unit may be omitted. If not omitted it must be the given unit ("B" by default).
 *
 * Examples: "10", "10B", "2.2M", "2.2 MB", "10 Mi", "10 MiB", "0.9 G"
 */
double parseSize(const std::string& str, const std::string& unit = "B");

} // namespace komb
