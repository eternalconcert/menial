#include "common.h"

std::string readFile(std::string path) {
    std::ifstream file(path);

    if (!file) {
        throw FileNotFoundException(path);
    }

    try {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    } catch (const std::ios_base::failure &) {
        throw FileNotFoundException(path);
    }
}

LogLevel logLevelByName(std::string levelName) {
    if (levelName == "highlight") {
        return HIGHLIGHT;
    }
    if (levelName == "debug") {
        return DEBUG;
    }
    else if (levelName == "info") {
        return INFO;
    }
    else if (levelName == "warning") {
        return WARNING;
    }
    else if (levelName == "error") {
        return ERROR;
    }
    else if (levelName == "critical") {
        return CRITICAL;
    }
    else {
        return NOTSET;
    }
};


std::string base64decode(std::string const &encoded) {
    std::string out;
    std::vector<int> T(256, -1);
    for (int i=0; i < 64; i++) {
        T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
    }
    int val = 0;
    int valb = -8;

    for (char c: encoded) {
        if (T[c] == -1) {
            break;
        }
        val = (val << 6) + T[c];
        valb += 6;

        if (valb >= 0) {
            out.push_back(char((val>>valb)&0xFF));
            valb -= 8;
        }
    }
    return out;
}


std::string sha256hash(const std::string input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);
    std::stringstream res;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        res << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return res.str();
}


std::string lower(std::string inStr) {
    std::transform(inStr.begin(), inStr.end(), inStr.begin(), ::tolower);
    return inStr;
}


std::map<std::string, std::string>* mimeTypeMap = 0;


std::string getMimeType(std::string extension, std::string filePath) {
    extension = lower(extension);
    if (mimeTypeMap == 0) {
        mimeTypeMap = new std::map<std::string, std::string>();

        std::istringstream fileContent(readFile(filePath));
        std::string line;
        while (std::getline(fileContent, line)) {
            std::string ext = line.substr(0, line.find_first_of(" "));
            std::string mimeType = line.substr(line.find_last_of(" ") + 1, std::string::npos);

            mimeTypeMap->operator[](ext) = mimeType;
        }
    }

    std::string fileType = mimeTypeMap->operator[](extension);
    if (fileType == "") {
        fileType = "text/html; charset=utf-8;";
    }
    return fileType;
}


std::string currentDateTime() {
    time_t now = time(0);
    struct tm timeStruct = *localtime(&now);
    char timeBuffer[29];

    strftime(timeBuffer, sizeof(timeBuffer), "%a, %d %m %Y %H:%M:%S GMT", &timeStruct);
    return timeBuffer;
};


std::string compressString(std::string str, std::string compression) {
  z_stream zs;
  memset(&zs, 0, sizeof(zs));


  if (compression == "gzip") {
    if(deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        throw(std::runtime_error("delateInit failed while compressing"));
    }
  } else if (compression == "deflate") {
    if(deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
        throw(std::runtime_error("delateInit failed while compressing"));
    }
  }

  zs.next_in = (Bytef*)str.data();
  zs.avail_in = str.size();

  int ret;
  char outbuffer[32768];
  std::string outstring;

  do {
      zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
      zs.avail_out = sizeof(outbuffer);

      ret = deflate(&zs, Z_FINISH);

      if (outstring.size() < zs.total_out) {
          outstring.append(outbuffer, zs.total_out - outstring.size());
      }
  } while (ret == Z_OK);

  deflateEnd(&zs);

  if (ret != Z_STREAM_END) {
      std::ostringstream oss;
      oss << "Exception during zlib compression";
      throw(std::runtime_error(oss.str()));
  }

  return outstring;

};