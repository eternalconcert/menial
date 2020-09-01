#include "fileresponse.h"


std::string FileResponse::makeEtag() {
    return sha256hash(this->getLastModified());
}


bool FileResponse::contentMatch() {
    if (this->config->debug) {
        // Early exit if debug mode is active
        return false;
    }
    std::string requestHeaders = this->getRequest()->getHeaders();
    std::string tag = "If-None-Match: ";

    bool result = false;
    if (requestHeaders.find(tag) != std::string::npos) {
        std::string hash;
        std::string eTag = this->makeEtag();
        int i = requestHeaders.find(tag) + tag.length();
        int j = 0;
        result = true;
        // Hacky comparson: No idea why == did not work.
        while (j < 64) {
            if (requestHeaders[i] != eTag[j]) {
                result = false;
            };
            i++;
            j++;
        }
    }

    return result;
}


void FileResponse::setFilePath() {
    std::string targetPath = this->target;
    if (targetPath.find("..") != std::string::npos) {
        this->logger->warning("Intrusion try detected: " + targetPath + " Resseting target to /");
        targetPath = "/";
    }

    targetPath = std::regex_replace(targetPath, std::regex("%20"), " ");

    std::string paramString = this->getRequest()->getGetParams();
    if (paramString.length() > 0) {
        targetPath.erase(targetPath.find(paramString));
    }

    if (targetPath == "/" and this->hostConfig["dirlisting"] == "false") {
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
    return this->headerBase() + this->getETag() + HEADERDELIM;
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
    if (this->status == 200) {
        header += this->getLastModified();
        header += this->getETag() + "\n";
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

    std::string cleanTarget = std::regex_replace(this->target, std::regex("%20"), " ");

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
        listing += "<li class=\"dirlisting dir\"><a href=" + std::regex_replace(dir, std::regex(" "), "%20") + "/>" + dir + "</a></li>\n";
    }

    for (std::vector<std::string>::iterator it = fileList.begin(); it != fileList.end(); ++it) {
        std::string file = *it;
        listing += "<li class=\"dirlisting file\"><a href=" + std::regex_replace(file, std::regex(" "), "%20")  + ">" + file + "</a></li>\n";
    }

    listing += "</ul>";
    std::string listTemplate = readFile(this->hostConfig["staticdir"] + "dirlisting.html");
    listTemplate = std::regex_replace(listTemplate, std::regex("<_DIR_>"), cleanTarget);
    listTemplate = std::regex_replace(listTemplate, std::regex("<_LISTING_>"), listing);
    return listTemplate;
}


std::string FileResponse::getContent() {
    std::string filePath = this->filePath;
    std::string content;
    if (this->hostConfig["dirlisting"] == "true") {
        try {
            std::string targetPath = this->target;
            if (targetPath.rfind("/") == (targetPath.length() - 1)) {
                if (targetPath.find("..") != std::string::npos) {
                    this->logger->warning("Intrusion try detected: " + targetPath);
                    throw FileNotFoundException("Intrusion try detected: " + targetPath);
                }
                return this->getDirlisting();
            }
        } catch (const FileNotFoundException &) {
            this->setStatus(404);
            return readFile(this->hostConfig["staticdir"] + "404.html");
        }
    }

    try {
        content = readFile(filePath);
    } catch (const FileNotFoundException &) {
        this->setStatus(404);
        return readFile(this->hostConfig["staticdir"] + "404.html");
    }
    return content;
}

std::string FileResponse::getETag() {
    std::string Etag = "ETag: ";
    Etag += this->makeEtag();
    return Etag;
}

std::string FileResponse::getLastModified() {
    // Last-Modified: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
    char mtime[50];
    memset(mtime, 0, sizeof(mtime));
    struct stat fileInfo;
    stat(this->filePath.c_str(), &fileInfo);
    time_t t = fileInfo.st_mtime;
    struct tm lt;
    localtime_r(&t, &lt);
    strftime(mtime, sizeof(mtime), "Last-Modified: %a, %d %m %Y %H:%M:%S GMT\n", &lt);
    return std::string(mtime);
};
