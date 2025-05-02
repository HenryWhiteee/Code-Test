#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <memory>

struct TradeStats {
    long long maxTimeGap = 0; // 最大时间间隔
    long long totalVolume = 0; // 总交易量
    long long totalPrice = 0; // 总价格（用于计算加权平均价格）
    long long maxPrice = 0; // 最高交易价格
    long long lastTimestamp = -1; // 上一次交易的时间戳
};

// 读csv
template <typename T>
std::vector<T> parseCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<T> data;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        T record;
        std::string field;
        while (std::getline(ss, field, ',')) {
            record.push_back(field);
        }
        data.push_back(record);
    }
    return data;
}

// 计算统计数据
std::map<std::string, TradeStats> calculateStats(const std::vector<std::vector<std::string>>& trades) {
    std::map<std::string, TradeStats> stats;
    for (const auto& trade : trades) {
        long long timestamp = std::stoll(trade[0]);
        std::string symbol = trade[1];
        long long quantity = std::stoll(trade[2]);
        long long price = std::stoll(trade[3]);
        
        auto& stat = stats[symbol];
        if (stat.lastTimestamp != -1) {
            long long timeGap = timestamp - stat.lastTimestamp;
            stat.maxTimeGap = std::max(stat.maxTimeGap, timeGap); //最大时间间隔
        }
        stat.lastTimestamp = timestamp;
        stat.totalVolume += quantity; //总量
        stat.totalPrice += quantity * price;
        stat.maxPrice = std::max(stat.maxPrice, price); //最大价格
    }

    for (auto& [symbol, stat] : stats) {
        stat.totalPrice /= stat.totalVolume; //加权均价
    }

    return stats;
}

// 输出csv
void writeOutput(const std::map<std::string, TradeStats>& stats, const std::string& outputFilename) {
    std::ofstream file(outputFilename);
    for (const auto& [symbol, stat] : stats) {
        file << symbol << ',' << stat.maxTimeGap << ',' << stat.totalVolume << ','
             << stat.totalPrice << ',' << stat.maxPrice << '\n';
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.csv> <output.csv>" << std::endl;
        return 1;
    }
    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];
    auto trades = parseCSV<std::vector<std::string>>(inputFilename);
    auto stats = calculateStats(trades);
    writeOutput(stats, outputFilename);
    std::cout << "Processing complete. Output written to " << outputFilename << std::endl;
    return 0;
}