#include <dirent.h>
#include <ctime>
#include <cstring>
#include <regex>
#include <stdio.h>
#include <sys/stat.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "fileresponse.h"


static const std::string HEADERDELIM = "\n\n";

std::string FileResponse::makeEtag() {
    return sha256hash(this->getLastModified());
}


bool FileResponse::contentMatch() {
    std::string requestHeaders = this->getRequest()->getHeaders();
    std::string tag = "If-None-Match: ";

    bool result = false;
    if (requestHeaders.find(tag) != std::string::npos) {
        std::string hash;
        std::string eTag = this->makeEtag();
        int i = requestHeaders.find(tag) + tag.length();
        int j = 0;
        result = true;
        // Hacky comparson: No idea why == does not work.
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

    if (targetPath == "/" and this->config["dirlisting"] == "false") {
        targetPath = this->config["defaultdocument"];
        this->logger->debug("No file on / requested, using default target: " + targetPath);
    }
    else if (targetPath.back() == '/' and this->config["dirlisting"] == "false") {
            targetPath = targetPath + this->config["defaultdocument"];
            this->logger->debug("No file on subdir requested, using default target: " + targetPath);
    }
    this->logger->debug("Requested document: " + targetPath);

    this->filePath = this->config["root"] + targetPath;
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


std::string FileResponse::notFound() {
    this->logger->info("404: Unknown target requested: " + target);
    this->setStatus(404);
    std::string content = this->headerBase() + HEADERDELIM + readFile(this->config["staticdir"] + "404.html");
    return content;
}


std::string FileResponse::methodNotAllowed() {
    this->setStatus(405);
    std::string header = this->headerBase();
    return header;
}


std::string FileResponse::internalServerError() {
    this->logger->error("Internal server error");
    this->setStatus(500);
    std::string content = this->headerBase() + HEADERDELIM + readFile(this->config["staticdir"] + "500.html");
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
    header += this->config["additionalheaders"];
    header += "\r\n";
    this->logger->debug("FileResponse header: " + header);
    return header;
}


std::string FileResponse::guessFileType(std::string fileName) {
    std::string charset = " charset=utf-8;";
    std::string fileType = "text/html;" + charset;;

    std::string extension = fileName.substr(fileName.find_last_of(".") + 1, fileName.length());

    this->logger->debug("Filename extension: " + extension);
    if (extension == "css") {
        fileType = "text/css";
    }
    else if (extension == "pdf") {
        fileType = "application/pdf";
    }
    else if (extension == "log") {
        fileType = "text/plain";
    }
    else if (extension == "png") {
        fileType = "image/png";
    }
    else if (extension == "jpg") {
        fileType = "image/jpg";
    }
    else if (extension == "mp3") {
        fileType = "audio/mpeg";
    }
    else if (extension == "flac") {
        fileType = "audio/flac";
    }
    else if (extension == "tar") {
        fileType = "application/x-tar";
    }
    else if (extension == "zip") {
        fileType = "application/zip";
    }
    else if (extension == "gz") {
        fileType = "application/gzip";
    }
    else if (extension == "bin" or extension == "file" or extension == "com" or extension == "class") {
        fileType = "application/octet-stream";
    }
    return fileType;
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
    std::string listTemplate = readFile(this->config["staticdir"] + "dirlisting.html");
    listTemplate = std::regex_replace(listTemplate, std::regex("<_DIR_>"), cleanTarget);
    listTemplate = std::regex_replace(listTemplate, std::regex("<_LISTING_>"), listing);
    return listTemplate;
}


std::string FileResponse::getContent() {
    std::string filePath = this->filePath;
    std::string content;
    if (this->config["dirlisting"] == "true") {
        try {
            std::string targetPath = this->filePath;
            if (targetPath.rfind("/") == (targetPath.length() - 1)) {
                if (targetPath.find("..") != std::string::npos) {
                    this->logger->warning("Intrusion try detected: " + targetPath);
                    throw FileNotFoundException("Intrusion try detected: " + targetPath);
                }
                return this->getDirlisting();
            }
        } catch (FileNotFoundException) {
            this->setStatus(404);
            return readFile(this->config["staticdir"] + "404.html");
        }
    }

    try {
        content = readFile(filePath);
    } catch (FileNotFoundException) {
        this->setStatus(404);
        return readFile(this->config["staticdir"] + "404.html");
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
