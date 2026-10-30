// generate_test_data.cpp
// Synthetic CSV generator for the ad-campaign schema:
//   campaign_id,date,impressions,clicks,spend,conversions
//
// Build:  g++ -std=c++17 -O2 -o generate_test_data generate_test_data.cpp
// CLI:    ./generate_test_data [output_dir=test_data] [seed=42] [large_rows=100000]
//
// Library use (e.g. from a unit-test fixture):
//   #define GENERATE_TEST_DATA_NO_MAIN      // suppress main()
//   #include "generate_test_data.cpp"
//   generateTestData("tmp_data", 123, 5000); // returns true on success
//
// Every file respects the business invariants:
//   conversions <= clicks <= impressions, spend >= 0 (2 decimals), valid YYYY-MM-DD.

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <random>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// ------------------------------------------------------------------ model --
struct Row {
    std::string campaign_id;
    std::string date;          // YYYY-MM-DD
    long long   impressions;
    long long   clicks;
    double      spend;         // USD, 2 decimals
    long long   conversions;
};

struct Date {
    int y, m, d;
};

static bool isLeap(int y) { return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0; }

static int daysInMonth(int y, int m) {
    static const int dm[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return (m == 2 && isLeap(y)) ? 29 : dm[m - 1];
}

static Date nextDay(Date t) {
    if (++t.d > daysInMonth(t.y, t.m)) {
        t.d = 1;
        if (++t.m > 12) { t.m = 1; ++t.y; }
    }
    return t;
}

static Date addDays(Date t, int n) {
    while (n-- > 0) t = nextDay(t);
    return t;
}

static std::string fmtDate(const Date& t) {
    char buf[16];
    std::snprintf(buf, sizeof buf, "%04d-%02d-%02d", t.y, t.m, t.d);
    return buf;
}

static std::string campaignId(int n) {
    char buf[16];
    std::snprintf(buf, sizeof buf, "CMP%03d", n);
    return buf;
}

static double roundCents(double v) { return std::llround(v * 100.0) / 100.0; }

static Row makeRow(const std::string& id, const Date& d, long long imp,
                   long long clk, double spend, long long conv) {
    return Row{id, fmtDate(d), imp, clk, roundCents(spend), conv};
}

// ----------------------------------------------------------------- output --
// Header is ALWAYS written, so an empty vector produces a header-only CSV.
// No trailing newline / blank line is emitted after the last line.
static void writeCsv(const fs::path& path, const std::vector<Row>& rows) {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("Cannot open " + path.string() + " for writing");
    // Lines are separated by '\n' but the file does NOT end with one, so
    // readers that split on newlines never see a trailing blank row.
    out << "campaign_id,date,impressions,clicks,spend,conversions";
    out << std::fixed << std::setprecision(2);
    for (const auto& r : rows) {
        out << '\n' << r.campaign_id << ',' << r.date << ',' << r.impressions << ','
            << r.clicks << ',' << r.spend << ',' << r.conversions;
    }
    std::cout << "  " << std::left << std::setw(28) << path.filename().string()
              << rows.size() << " data row(s)\n";
}

// Completely empty (0 bytes) file: no header, no rows.
static void writeZeroByteFile(const fs::path& path) {
    std::ofstream out(path, std::ios::trunc);
    std::cout << "  " << std::left << std::setw(28) << path.filename().string()
              << "0 bytes (no header)\n";
}

// ------------------------------------------------------------- generators --
// Random but logically consistent metrics.
static Row randomRow(std::mt19937_64& rng, const std::string& id, const Date& d,
                     long long maxImpressions = 200000) {
    std::uniform_int_distribution<long long> impDist(0, maxImpressions);
    std::uniform_real_distribution<double> ctrDist(0.0, 0.10);
    std::uniform_real_distribution<double> cvrDist(0.0, 0.30);
    std::uniform_real_distribution<double> cpcDist(0.05, 3.00);

    long long imp  = impDist(rng);
    long long clk  = std::llround(imp * ctrDist(rng));
    long long conv = std::llround(clk * cvrDist(rng));
    double spend   = clk * cpcDist(rng);
    return makeRow(id, d, imp, clk, spend, conv);
}

// 1. Empty vector -> header-only CSV
static std::vector<Row> genEmpty() { return {}; }

// 2. Exactly one row
static std::vector<Row> genSingleRow() {
    return {makeRow("CMP001", {2025, 1, 1}, 12000, 300, 45.50, 12)};
}

// 3. Example from the specification
static std::vector<Row> genSpecExample() {
    return {
        makeRow("CMP001", {2025, 1, 1}, 12000, 300, 45.50, 12),
        makeRow("CMP002", {2025, 1, 1}, 8000, 120, 28.00, 4),
        makeRow("CMP001", {2025, 1, 2}, 14000, 340, 48.20, 15),
        makeRow("CMP003", {2025, 1, 1}, 5000, 60, 15.00, 3),
        makeRow("CMP002", {2025, 1, 2}, 8500, 150, 31.00, 5),
    };
}

// 4. Zero metrics: no traffic, clicks without conversions, spend without conversions
static std::vector<Row> genZeroMetrics() {
    return {
        makeRow("CMP001", {2025, 3, 1}, 0, 0, 0.00, 0),         // everything zero
        makeRow("CMP001", {2025, 3, 2}, 1000, 0, 0.00, 0),      // impressions only
        makeRow("CMP001", {2025, 3, 3}, 1000, 50, 12.50, 0),    // clicks, no conversions
        makeRow("CMP002", {2025, 3, 1}, 1000, 50, 0.00, 5),     // free clicks (spend = 0)
        makeRow("CMP002", {2025, 3, 2}, 0, 0, 0.00, 0),
    };
}

// 5. Boundary values of the invariants and very large / very small numbers
static std::vector<Row> genBoundaries() {
    return {
        makeRow("CMP001", {2025, 4, 1}, 1, 1, 0.01, 1),                     // minimum non-zero
        makeRow("CMP001", {2025, 4, 2}, 100, 100, 99.99, 100),              // clicks == impressions == conversions
        makeRow("CMP002", {2025, 4, 1}, 1000, 1000, 500.00, 0),             // CTR = 100%
        makeRow("CMP002", {2025, 4, 2}, 1000, 10, 5.00, 10),                // CVR = 100%
        makeRow("CMP003", {2025, 4, 1}, 2147483647LL, 1000000, 999999.99, 50000),   // INT32_MAX impressions
        makeRow("CMP003", {2025, 4, 2}, 4294967296LL, 2147483648LL, 1234567.89, 100000), // > INT32_MAX
        makeRow("CMP004", {2025, 4, 1}, 1000000, 1, 0.01, 0),               // tiny CTR, tiny spend
        makeRow("CMP005", {2025, 4, 1}, 7, 3, 0.10, 1),                     // spend with trailing zero
        makeRow("CMP005", {2025, 4, 2}, 7, 3, 1234.50, 1),                  // one decimal digit in source
    };
}

// 6. Calendar edge cases
static std::vector<Row> genDateEdges() {
    std::vector<Date> dates = {
        {2024, 2, 28}, {2024, 2, 29}, {2024, 3, 1},     // leap year
        {2000, 2, 29},                                  // leap (divisible by 400)
        {2025, 2, 28}, {2025, 3, 1},                    // non-leap year
        {2025, 1, 31}, {2025, 4, 30},                   // month ends
        {2025, 12, 31}, {2026, 1, 1},                   // year rollover
        {1970, 1, 1}, {2099, 12, 31},                   // far past / far future
    };
    std::vector<Row> rows;
    int i = 0;
    for (const auto& d : dates) {
        rows.push_back(makeRow(campaignId(1), d, 1000 + 10 * i, 50 + i, 10.0 + i, 2 + (i % 3)));
        ++i;
    }
    return rows;
}

// 7. Several campaigns over a continuous date range, sorted by date then campaign
static std::vector<Row> genMultiCampaign(std::mt19937_64& rng, int campaigns, int days) {
    std::vector<Row> rows;
    Date start{2025, 1, 1};
    for (int d = 0; d < days; ++d) {
        Date cur = addDays(start, d);
        for (int c = 1; c <= campaigns; ++c)
            rows.push_back(randomRow(rng, campaignId(c), cur));
    }
    return rows;
}

// 8. Same data as above but in random order (parsers must not assume sorting)
static std::vector<Row> genUnsorted(std::mt19937_64& rng) {
    auto rows = genMultiCampaign(rng, 4, 14);
    std::shuffle(rows.begin(), rows.end(), rng);
    return rows;
}

// 9. Same (campaign_id, date) key repeated (e.g. multiple hourly exports)
static std::vector<Row> genDuplicateKeys() {
    return {
        makeRow("CMP001", {2025, 5, 1}, 1000, 40, 10.00, 2),
        makeRow("CMP001", {2025, 5, 1}, 1000, 40, 10.00, 2),   // exact duplicate
        makeRow("CMP001", {2025, 5, 1}, 2500, 90, 22.40, 5),   // same key, different values
        makeRow("CMP002", {2025, 5, 1}, 3000, 75, 18.00, 3),
    };
}

// 10. Sparse data: campaigns with different lifetimes, gaps and one-day campaigns
static std::vector<Row> genSparse(std::mt19937_64& rng) {
    std::vector<Row> rows;
    Date start{2025, 6, 1};
    // CMP001: 10 consecutive days
    for (int d = 0; d < 10; ++d) rows.push_back(randomRow(rng, campaignId(1), addDays(start, d)));
    // CMP002: every 3rd day (gaps)
    for (int d = 0; d < 30; d += 3) rows.push_back(randomRow(rng, campaignId(2), addDays(start, d)));
    // CMP003: single day only
    rows.push_back(randomRow(rng, campaignId(3), addDays(start, 15)));
    // CMP004: starts late
    for (int d = 20; d < 25; ++d) rows.push_back(randomRow(rng, campaignId(4), addDays(start, d)));
    // CMP1000: ID longer than the usual 6 characters
    rows.push_back(randomRow(rng, "CMP1000", addDays(start, 5)));
    return rows;
}

// 11. Large random dataset for performance tests
static std::vector<Row> genLarge(std::mt19937_64& rng, size_t n) {
    std::vector<Row> rows;
    rows.reserve(n);
    std::uniform_int_distribution<int> campDist(1, 200);
    std::uniform_int_distribution<int> dayDist(0, 729);   // ~2 years
    Date start{2024, 1, 1};
    for (size_t i = 0; i < n; ++i)
        rows.push_back(randomRow(rng, campaignId(campDist(rng)), addDays(start, dayDist(rng)), 5000000));
    return rows;
}

// -------------------------------------------------------- public function --
// Generates all test CSV files into outDir (created if missing).
// Returns true on success, false if any file could not be written.
bool generateTestData(const fs::path& outDir = "test_data",
                      uint64_t seed = 42,
                      size_t largeRows = 100000) {
    try {
        fs::create_directories(outDir);
        std::mt19937_64 rng(seed);

        std::cout << "Writing test data to '" << outDir.string() << "' (seed=" << seed << ")\n";

        writeCsv(outDir / "empty_header_only.csv",      genEmpty());     // vector size == 0
        writeZeroByteFile(outDir / "empty_zero_bytes.csv");              // no header at all
        writeCsv(outDir / "single_row.csv",             genSingleRow());
        writeCsv(outDir / "spec_example.csv",           genSpecExample());
        writeCsv(outDir / "zero_metrics.csv",           genZeroMetrics());
        writeCsv(outDir / "boundary_values.csv",        genBoundaries());
        writeCsv(outDir / "date_edge_cases.csv",        genDateEdges());
        writeCsv(outDir / "multi_campaign_sorted.csv",  genMultiCampaign(rng, 5, 30));
        writeCsv(outDir / "unsorted.csv",               genUnsorted(rng));
        writeCsv(outDir / "duplicate_keys.csv",         genDuplicateKeys());
        writeCsv(outDir / "sparse_campaigns.csv",       genSparse(rng));
        writeCsv(outDir / "large_random.csv",           genLarge(rng, largeRows));

        std::cout << "Done.\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "generateTestData failed: " << e.what() << '\n';
        return false;
    }
}

int main(int argc, char** argv) {
    fs::path outDir = (argc > 1) ? argv[1] : "test_data";
    uint64_t seed   = (argc > 2) ? std::strtoull(argv[2], nullptr, 10) : 42;
    size_t largeN   = (argc > 3) ? std::strtoull(argv[3], nullptr, 10) : 100000;
    return generateTestData(outDir, seed, largeN) ? 0 : 1;
}
