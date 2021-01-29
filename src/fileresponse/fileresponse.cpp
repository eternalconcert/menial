#include "fileresponse.h"


std::string FileResponse::makeEtag() {
    return sha256hash(this->getLastModifiedHeader());
}


bool FileResponse::contentMatch() {
    if (this->config->debug) {
        // Early exit if debug mode is active
        return false;
    }
    std::string requestHeaders = this->getRequest()->getHeaders();
    std::string ifNoneMatchTag = "If-None-Match: ";

    bool eTagsMatch = false;
    if (requestHeaders.find(ifNoneMatchTag) != std::string::npos) {
        std::string hash;
        std::string eTag = this->makeEtag();
        int i = requestHeaders.find(ifNoneMatchTag) + ifNoneMatchTag.length();
        int j = 0;
        eTagsMatch = true;
        // Hacky comparson: No idea why == did not work.
        while (j < 64) {
            if (requestHeaders[i] != eTag[j]) {
                eTagsMatch = false;
            };
            i++;
            j++;
        }
    }

    if (eTagsMatch == true) {
        return true;
    };

    std::string modifiedSinceHeader = "If-Modified-Since: ";
    if (requestHeaders.find(modifiedSinceHeader) != std::string::npos) {
        bool modifiedSinceHeaderMatch = false;
        int headerEndPos = requestHeaders.find(modifiedSinceHeader) + modifiedSinceHeader.length();
        std::string modifiedSinceHeaderValue = requestHeaders.substr(headerEndPos, 28);
        time_t requestedTime;
        struct tm rtTimestruct;
        strptime(modifiedSinceHeaderValue.c_str(), "%a, %d %m %Y %H:%M:%S GMT", &rtTimestruct);
        requestedTime = mktime(&rtTimestruct);
        time_t ownTime;
        struct tm otTimestruct;
        strptime(this->getLastModifiedTime().c_str(), "%a, %d %m %Y %H:%M:%S GMT", &otTimestruct);
        ownTime = mktime(&otTimestruct);
        modifiedSinceHeaderMatch = difftime(requestedTime, ownTime) < 0.0;

        if (!modifiedSinceHeaderMatch) {
            return true;
        };
    }

    return false;
}


void FileResponse::setFilePath() {
    std::string targetPath = this->getCleanTarget();
    if (targetPath.find("..") != std::string::npos) {
        this->logger->warning("Intrusion try detected: " + targetPath + " Resseting target to /");
        targetPath = "/";
    }

    targetPath = std::regex_replace(targetPath, std::regex("%20"), " ");

    std::string rootPrefix = this->hostConfig["rootPrefix"];
    if (rootPrefix.length() != 0) {
        targetPath = std::regex_replace(targetPath, std::regex(rootPrefix.c_str()), "/");
    };

    if (targetPath == "/" && this->hostConfig["dirlisting"] == "false") {
        targetPath = this->hostConfig["defaultdocument"];
        this->logger->debug("No file on / requested, using default target: " + targetPath);
    }
    else if (targetPath.back() == '/' and this->hostConfig["dirlisting"] == "false") {
        targetPath = targetPath + this->hostConfig["defaultdocument"];
        this->logger->debug("No file on subdir requested, using default target: " + targetPath);
    }
    this->logger->debug("Requested document: " + targetPath);

    this->filePath = this->hostConfig["root"] + targetPath;
    this->logger->debug("Filepath: " + this->filePath);
};


std::string FileResponse::notModified() {
    this->logger->debug("Content has not changed, -> 304");
    this->setStatus(304);
    return this->headerBase() + this->getETagHeader() + "\n" + this->getLastModifiedHeader() + HEADERDELIM;
}


std::string FileResponse::head() {
    if (this->contentMatch()) {
        return this->notModified();
    }

    std::string fileName = this->fileName;

    std::string result;
    std::string content = this->getContent();
    result = this->getHeader(content, fileName);
    return result;
}


std::string FileResponse::options() {
    return this->empty();
}


std::string FileResponse::get() {
    if (this->contentMatch()) {
        return this->notModified();
    }

    std::string requestAcceptEncoding = this->getRequest()->getEncodings();

    if (requestAcceptEncoding.length() > 0) {
        if (requestAcceptEncoding.find("gzip") != std::string::npos) {
            this->compression = "gzip";
            this->logger->debug("Using compression: gzip");
        } else  if (requestAcceptEncoding.find("deflate") != std::string::npos) {
            this->compression = "deflate";
            this->logger->debug("Using compression: deflate");
        };
    }
    std::string fileName = this->fileName;
    std::string result;
    std::string content = this->getContent();

    result = this->getHeader(content, fileName) + content;

    return result;
}


std::string FileResponse::post() {
    return this->methodNotAllowed();
}


std::string FileResponse::notFound() {
    this->logger->info("404: Unknown target requested: " + target);
    this->setStatus(404);
    std::string content = this->headerBase() + HEADERDELIM + readFile(this->hostConfig["staticdir"] + "404.html");
    return content;
}


std::string FileResponse::getHeader(std::string content, std::string fileName) {
    std::string header = this->headerBase();
    header += "Content-Length: " + std::to_string(content.length()) + "\n";
    header += "Content-Type: " + this->guessFileType(fileName) + "\n";
    if (this->compression.length() > 0) {
        header += "Content-Encoding: " + this->compression + "\n";
    }
    if (this->status == 200) {
        header += this->getLastModifiedHeader() + "\n";
        header += this->getETagHeader() + "\n";
    }
    header += this->hostConfig["additionalheaders"];
    header += "\r\n";
    this->logger->debug("FileResponse header: " + header);
    return header;
}


std::string FileResponse::guessFileType(std::string fileName) {
    std::string extension = fileName.substr(0, fileName.find_last_of("?"));
    extension = extension.substr(extension.find_last_of(".") + 1, extension.length());
    this->logger->debug("Filename extension: " + extension);

    return getMimeType(extension, this->config->resources + "mimetypes.tray");
}


std::string FileResponse::getDirlisting() {
    std::string targetDir = this->filePath;
    targetDir = std::regex_replace(targetDir, std::regex("%20"), " ");
    this->logger->debug("Dirlisting is active and no specific file requested. Return dirlisting: " + targetDir);

    std::string cleanTarget = std::regex_replace(this->getCleanTarget(), std::regex("%20"), " ");

    std::vector<std::string> dirList;
    std::vector<std::string> fileList;
    std::string listing = "<ul class=\"dirlisting list\"><li class=\"dirlisting linkparent\"><a href=\"..\">Parent directory..</a></li>\n";
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(targetDir.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {

            std::string entityName = std::string(ent->d_name);
            if (entityName != "." and entityName != "..") {
                struct stat path_stat;
                std::string path = targetDir + std::string(ent->d_name);
                stat(path.c_str(), &path_stat);

                if (S_ISREG(path_stat.st_mode) == 0) {
                    dirList.push_back(entityName);
                } else {
                    fileList.push_back(entityName);
                }
            }
        }
        std::sort(dirList.begin(), dirList.end());
        std::sort(fileList.begin(), fileList.end());
    }

    for (std::vector<std::string>::iterator it = dirList.begin(); it != dirList.end(); ++it) {
        std::string dir = *it;
        std::string hidden = dir.front() == '.' ? " hidden" : "";
        listing += "<li class=\"dirlisting dir" + hidden + "\"><a href=" + std::regex_replace(dir, std::regex(" "), "%20") + "/>" + dir + "</a></li>\n";
    }

    for (std::vector<std::string>::iterator it = fileList.begin(); it != fileList.end(); ++it) {
        std::string file = *it;
        std::string hidden = file.front() == '.' ? " hidden" : "";
        listing += "<li class=\"dirlisting file" + hidden + "\"><a href=" + std::regex_replace(file, std::regex(" "), "%20")  + ">" + file + "</a></li>\n";
    }

    listing += "</ul>";
    std::string listTemplate = readFile(this->hostConfig["staticdir"] + "dirlisting.html");
    listTemplate = std::regex_replace(listTemplate, std::regex("<_DIR_>"), cleanTarget);
    listTemplate = std::regex_replace(listTemplate, std::regex("<_LISTING_>"), listing);
    return listTemplate;
}

std::string FileResponse::make404() {
    this->setStatus(404);
    return readFile(this->hostConfig["staticdir"] + "404.html");
}


std::string FileResponse::getCleanTarget() {
    std::string targetPath = this->target;
    std::string paramString = this->getRequest()->getGetParams();
    if (paramString.length() > 0) {
        targetPath.erase(targetPath.find(paramString));
    }
    return targetPath;
}


std::string FileResponse::getContent() {

    std::string filePath = this->filePath;
    std::string content;
    if (this->hostConfig["dirlisting"] == "true") {
        try {
            std::string targetPath = this->getCleanTarget();
            if (targetPath.rfind("/") == (targetPath.length() - 1)) {
                if (targetPath.find("..") != std::string::npos) {
                    this->logger->warning("Intrusion try detected: " + targetPath);
                    throw FileNotFoundException("Intrusion try detected: " + targetPath);
                }
                content = this->getDirlisting();
            }
        } catch (const FileNotFoundException &) {
            content = make404();
        }
    }
    if (content.empty()) {
        try {
            content = readFile(filePath);
            this->setStatus(200);
        } catch (const FileNotFoundException &) {
            std::string defaultFile = this->hostConfig["defaultfile"];
            if (defaultFile == "") {
                content = make404();
            } else {
                try {
                    content = readFile(this->hostConfig["root"] + defaultFile);
                    this->setStatus(200);
                } catch (const FileNotFoundException &) {
                    content = make404();
                }
            }

            std::string fallbackFile = this->hostConfig["fallback"];
            if (fallbackFile == "") {
                content = make404();
            } else {
                try {
                    content = readFile(fallbackFile);
                    this->setStatus(200);
                } catch (const FileNotFoundException &) {
                    this->logger->warning(
                        "No fallback file could be found: " + fallbackFile
                    );
                    content = make404();

                }
            }

        }
    }

    if (this->compression.length() > 0) {
        content = compressString(content, this->compression);
    }

    return content;
}

std::string FileResponse::getETagHeader() {
    std::string Etag = "ETag: ";
    Etag += this->makeEtag();
    return Etag;
}



std::string FileResponse::getLastModifiedTime() {
    // Last-Modified: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
    char mtime[29];
    memset(mtime, 0, sizeof(mtime));
    struct stat fileInfo;
    stat(this->filePath.c_str(), &fileInfo);
    time_t t = fileInfo.st_mtime;
    struct tm lt;
    localtime_r(&t, &lt);
    strftime(mtime, sizeof(mtime), "%a, %d %m %Y %H:%M:%S GMT", &lt);
    return std::string(mtime);
}

std::string FileResponse::getLastModifiedHeader() {
    return "Last-Modified: " + this->getLastModifiedTime();
};
