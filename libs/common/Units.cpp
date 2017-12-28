#include "Units.hpp"

#include <string>
#include <unordered_map>

#include <boost/algorithm/string.hpp>

#include <common/Logging.hpp>
#include <common/String.hpp>

namespace komb {

std::string numberWithSiPrefix(double num)
{
    const auto kSiPrefixes = {"", "k", "M", "G", "T", "P", "E", "Z"};
    for (const char* unit : kSiPrefixes)
    {
        if (std::abs(num) < 1000.0)
        {
            return strprintf("%3.2f %s", num, unit);
        }
        num /= 1000.0;
    }
    return strprintf("%.2f %s", num, "Y");
}

std::string sizeWithSiPrefix(size_t num)
{
    if (num < 1000) { return strprintf("%lu ", num); } // Save us from "42.0"
    return numberWithSiPrefix(static_cast<double>(num));
}

double prefixNumberFromString(const std::string& prefix)
{
    if (prefix == "")   {return 1.0;}
    if (prefix == "k")  {return 1000.0;}
    if (prefix == "M")  {return 1000.0 * 1000.0;}
    if (prefix == "G")  {return 1000.0 * 1000.0 * 1000.0;}
    if (prefix == "T")  {return 1000.0 * 1000.0 * 1000.0 * 1000.0;}
    if (prefix == "P")  {return 1000.0 * 1000.0 * 1000.0 * 1000.0 * 1000.0;}
    if (prefix == "Ki") {return 1024.0;}
    if (prefix == "Mi") {return 1024.0 * 1024.0;}
    if (prefix == "Gi") {return 1024.0 * 1024.0 * 1024.0;}
    if (prefix == "Ti") {return 1024.0 * 1024.0 * 1024.0 * 1024.0;}
    if (prefix == "Pi") {return 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0;}
    ABORT_F("Unknown SI prefix '%s'", prefix.c_str());
}

double parseSize(const std::string& str_arg, const std::string& unit)
{
    try
    {
        size_t number_end = 0;
        const double number = std::stod(str_arg, &number_end);
        CHECK_NE(number_end, 0u);

        std::string prefix_str = str_arg.substr(number_end);
        boost::algorithm::trim(prefix_str);
        if (!unit.empty() && boost::ends_with(prefix_str, unit))
        {
            prefix_str = prefix_str.substr(0, prefix_str.size() - unit.size());
            boost::algorithm::trim(prefix_str);
        }
        double prefix = prefixNumberFromString(prefix_str);
        return number * prefix;
    }
    catch (std::exception& e)
    {
        ABORT_F("Failed to parse size '%s': %s", str_arg.c_str(), e.what());
    }
}

} // namespace komb
