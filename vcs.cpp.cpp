#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <chrono>
#include <sstream>

namespace fs = std::filesystem;
using namespace std;

class LocalVCS {
    string vcsDir = ".lvcs";

    string hashFile(const string& path) const {
        ifstream file(path, ios::binary | ios::ate);
        if (!file) return "";
        streamsize size = file.tellg();
        file.seekg(0, ios::beg);
        
        string content(size, '\0');
        if (file.read(&content[0], size)) {
            stringstream hex_stream;
            hex_stream << hex << hash<string>{}(content);
            return hex_stream.str();
        }
        return "";
    }

public:
    void init() const {
        if (fs::exists(vcsDir)) return;
        fs::create_directories(vcsDir + "/objects");
        fs::create_directories(vcsDir + "/commits");
        ofstream(vcsDir + "/index");
        ofstream(vcsDir + "/HEAD");
    }

    void add(const string& filePath) const {
        if (!fs::exists(filePath) || fs::is_directory(filePath)) return;
        
        string fileHash = hashFile(filePath);
        if (fileHash.empty()) return;
        
        string objPath = vcsDir + "/objects/" + fileHash;
        if (!fs::exists(objPath)) {
            fs::copy_file(filePath, objPath, fs::copy_options::overwrite_existing);
        }
        
        unordered_map<string, string> indexMap;
        ifstream idxIn(vcsDir + "/index");
        string p, h;
        while (idxIn >> p >> h) {
            indexMap[p] = h;
        }
        idxIn.close();
        
        indexMap[filePath] = fileHash;
        
        ofstream idxOut(vcsDir + "/index", ios::trunc);
        for (const auto& [k, v] : indexMap) {
            idxOut << k << " " << v << "\n";
        }
    }

    void commit(const string& message) const {
        ifstream idxIn(vcsDir + "/index");
        if (idxIn.peek() == ifstream::traits_type::eof()) return;

        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string commitId = to_string(now);
        
        ofstream commitFile(vcsDir + "/commits/" + commitId);
        commitFile << message << "\n";
        commitFile << idxIn.rdbuf();
        idxIn.close();
        
        ofstream head(vcsDir + "/HEAD", ios::trunc);
        head << commitId;
        
        ofstream(vcsDir + "/index", ios::trunc);
    }

    void log() const {
        if (!fs::exists(vcsDir + "/commits")) return;
        
        for (const auto& entry : fs::directory_iterator(vcsDir + "/commits")) {
            ifstream cFile(entry.path());
            string msg;
            getline(cFile, msg);
            cout << entry.path().filename().string() << " : " << msg << "\n";
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    
    LocalVCS vcs;
    string cmd = argv[1];
    
    if (cmd == "init") {
        vcs.init();
    } 
    else if (cmd == "add" && argc > 2) {
        vcs.add(argv[2]);
    } 
    else if (cmd == "commit" && argc > 2) {
        vcs.commit(argv[2]);
    } 
    else if (cmd == "log") {
        vcs.log();
    }
    
    return 0;
}