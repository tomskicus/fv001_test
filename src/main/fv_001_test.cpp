// campaign_analytics.cpp
// Aggregates ad-campaign rows by campaign_id and produces two ranked lists:
//   A. Top 10 by highest CTR           -> top10_ctr.csv
//   B. Top 10 by lowest CPA (conv > 0) -> top10_cpa.csv
//
// Build:  g++ -std=c++17 -O2 -o campaign_analytics campaign_analytics.cpp
// CLI:    ./campaign_analytics <input.csv> [output_dir=.]
//
// Library use (e.g. unit tests):
//   #define CAMPAIGN_ANALYTICS_NO_MAIN
//   #include "campaign_analytics.cpp"
//   auto [topCtr, topCpa] = analyzeCampaigns(rows);   // two vectors
 
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
 
namespace fs = std::filesystem;
 
// ------------------------------------------------------------------ types --
struct Row {                       // one line of the input CSV
    std::string campaign_id;
    std::string date;
    long long   impressions = 0;
    long long   clicks      = 0;
    double      spend       = 0.0;
    long long   conversions = 0;
};
 
struct CampaignStats {             // one line of an output CSV
    std::string campaign_id;
    long long   total_impressions = 0;
    long long   total_clicks      = 0;
    double      total_spend       = 0.0;
    long long   total_conversions = 0;
    double      ctr               = 0.0;           // clicks / impressions (0 if no impressions)
    std::optional<double> cpa;                     // spend / conversions (nullopt if no conversions)
};
 
// ---------------------------------------------------------- core function --
// Aggregates by campaign_id and returns { top10 highest CTR, top10 lowest CPA }.
// Ties are broken by campaign_id (ascending) so results are deterministic.
// An empty input gives two empty vectors.
std::pair<std::vector<CampaignStats>, std::vector<CampaignStats>>
analyzeCampaigns(const std::vector<Row>& rows, size_t topN = 10) {
    // Spend is summed in integer cents to avoid floating-point drift.
    struct Acc { long long imp = 0, clk = 0, cents = 0, conv = 0; };
    std::map<std::string, Acc> acc;
    for (const auto& r : rows) {
        Acc& a = acc[r.campaign_id];
        a.imp   += r.impressions;
        a.clk   += r.clicks;
        a.cents += std::llround(r.spend * 100.0);
        a.conv  += r.conversions;
    }
 
    std::vector<CampaignStats> all;
    all.reserve(acc.size());
    for (const auto& [id, a] : acc) {
        CampaignStats s;
        s.campaign_id       = id;
        s.total_impressions = a.imp;
        s.total_clicks      = a.clk;
        s.total_spend       = a.cents / 100.0;
        s.total_conversions = a.conv;
        s.ctr = a.imp > 0 ? static_cast<double>(a.clk) / a.imp : 0.0;
        if (a.conv > 0) s.cpa = s.total_spend / a.conv;   // else stays null
        all.push_back(std::move(s));
    }
 
    // A. highest CTR
    std::vector<CampaignStats> byCtr = all;
    std::sort(byCtr.begin(), byCtr.end(), [](const CampaignStats& x, const CampaignStats& y) {
        if (x.ctr != y.ctr) return x.ctr > y.ctr;
        return x.campaign_id < y.campaign_id;
    });
    if (byCtr.size() > topN) byCtr.resize(topN);
 
    // B. lowest CPA, campaigns with zero conversions excluded
    std::vector<CampaignStats> byCpa;
    for (const auto& s : all) if (s.cpa) byCpa.push_back(s);
    std::sort(byCpa.begin(), byCpa.end(), [](const CampaignStats& x, const CampaignStats& y) {
        if (*x.cpa != *y.cpa) return *x.cpa < *y.cpa;
        return x.campaign_id < y.campaign_id;
    });
    if (byCpa.size() > topN) byCpa.resize(topN);
 
    return {std::move(byCtr), std::move(byCpa)};
}
 
// -------------------------------------------------------------------- I/O --
// Reads the input CSV (header line skipped; blank lines ignored; empty or
// header-only files give an empty vector).
std::vector<Row> readRows(const fs::path& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Cannot open " + path.string());
 
    std::vector<Row> rows;
    std::string line;
    bool first = true;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        if (first) { first = false; continue; }          // header
 
        std::stringstream ss(line);
        std::string f[6];
        for (auto& x : f)
            if (!std::getline(ss, x, ',')) throw std::runtime_error("Malformed line: " + line);
        Row r;
        try {
            r.campaign_id = f[0];
            r.date        = f[1];
            r.impressions = std::stoll(f[2]);
            r.clicks      = std::stoll(f[3]);
            r.spend       = std::stod(f[4]);
            r.conversions = std::stoll(f[5]);
        } catch (const std::exception&) {
            throw std::runtime_error("Malformed line: " + line);
        }
        rows.push_back(std::move(r));
    }
    return rows;
}
 
// Writes stats as CSV. Header always written; no trailing newline.
// A null CPA is written as an empty field.
void writeStats(const fs::path& path, const std::vector<CampaignStats>& stats) {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("Cannot open " + path.string() + " for writing");
 
    out << "campaign_id,total_impressions,total_clicks,total_spend,total_conversions,CTR,CPA";
    for (const auto& s : stats) {
        out << '\n' << s.campaign_id << ',' << s.total_impressions << ',' << s.total_clicks << ','
            << std::fixed << std::setprecision(2) << s.total_spend << ','
            << s.total_conversions << ','
            << std::setprecision(4) << s.ctr << ',';
        if (s.cpa) out << std::setprecision(2) << *s.cpa;
    }
}
 
// ------------------------------------------------------------- CLI wrapper --
#ifndef CAMPAIGN_ANALYTICS_NO_MAIN
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.csv> [output_dir=.]\n";
        return 1;
    }
    try {
        fs::path outDir = (argc > 2) ? argv[2] : ".";
        fs::create_directories(outDir);
 
        auto [topCtr, topCpa] = analyzeCampaigns(readRows(argv[1]));
        writeStats(outDir / "top10_ctr.csv", topCtr);
        writeStats(outDir / "top10_cpa.csv", topCpa);
 
        std::cout << "top10_ctr.csv: " << topCtr.size() << " row(s)\n"
                  << "top10_cpa.csv: " << topCpa.size() << " row(s)\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
#endif
 
