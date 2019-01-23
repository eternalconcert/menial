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


void FileResponse::setGetParamsString() {

    std::string header = this->getRequest()->getHeader();
    header.erase(0, header.find(" ") + 1);
    header.erase(header.find(" "), header.length());

    std::string paramString = header;
    paramString.erase(0, header.find("?"));
    this->logger->debug("Get Request params " + paramString);
    this->paramString = paramString;
};


void FileResponse::setFilePath() {
    std::string targetPath = this->target;
    if (targetPath.find("..") != std::string::npos) {
        this->logger->warning("Intrusion try detected: " + targetPath + " Resseting target to /");
        targetPath = "/";
    }

    targetPath = std::regex_replace(targetPath, std::regex("%20"), " ");

    if (this->paramString.length() > 0) {
        targetPath.erase(targetPath.find(this->paramString));
    }

    if (targetPath == "/") {
        targetPath = this->config["defaultdocument"];
        this->logger->debug("No file on / requested, using default target: " + targetPath);
    }
    else if (targetPath.back() == '/') {
            targetPath = targetPath + this->config["defaultdocument"];
            this->logger->debug("No file on subdir requested, using default target: " + targetPath);
    }
    this->logger->debug("Requested document: " + targetPath);

    this->filePath = this->config["root"] + targetPath;
    this->logger->debug("Filepath: " + this->filePath);
};


std::string FileResponse::headerBase() {
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Server: menial\n";
    return header;
}


std::string FileResponse::head() {
    std::string fileName = this->fileName;

    std::string result;
    std::string content = this->getContent();
    result = this->getHeader(content, fileName);
    return result;
}


std::string FileResponse::get() {
    return this->head() + this->getContent();
}


std::string FileResponse::methodNotAllowed() {
    this->setStatus(405);
    std::string header = this->headerBase();
    return header;
}


std::string FileResponse::getHeader(std::string content, std::string fileName) {
    std::string header = this->headerBase();
    header += "Content-Length: " + std::to_string(content.length()) + "\n";
    header += "Content-Type: " + this->guessFileType(fileName) + "\n";
    if (this->status == 200) {
        header += this->getLastModified();
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
    else if (extension == "gz") {
        fileType = "application/gzip";
    }
    else if (extension == "bin" or extension == "file" or extension == "com" or extension == "class") {
        fileType = "application/octet-stream";
    }
    return fileType;
}


std::string FileResponse::make404() {
    this->setStatus(404);
    this->logger->info("404: Unknown target requested: " + target);
    return readFile(this->config["errorPagesDir"] + "404.html");
}


std::string FileResponse::getContent() {
    std::string filePath = this->filePath;
    if (this->paramString.length() > 0) {
        filePath.erase(filePath.find(this->paramString));
    }

    std::string content;

    if (this->config["dirlisting"] == "true") {
        try {
            std::string targetPath = this->target;
            if (targetPath.rfind("/") == (targetPath.length() - 1)) {
                if (targetPath.find("..") != std::string::npos) {
                    this->logger->warning("Intrusion try detected: " + targetPath);
                    throw FileNotFoundException("Intrusion try detected: " + targetPath);
                }
                std::string targetDir = this->config["root"] + targetPath.substr(1, targetPath.size() - 1);
                targetDir = std::regex_replace(targetDir, std::regex("%20"), " ");
                this->logger->debug("Dirlisting is active and no specific file requested. Return dirlisting: " + targetDir);
                DIR *dir;
                struct dirent *ent;
                if ((dir = opendir(targetDir.c_str())) != NULL) {
                    content += "<h1>Content of " + std::regex_replace(targetPath, std::regex("%20"), " ") + "</h1>";
                    while ((ent = readdir(dir)) != NULL) {
                        std::string dirName = std::string(ent->d_name);
                        std::string term = "/>";
                        if (dirName.find(".") != std::string::npos) {
                            term = ">";
                        }
                        content += "<a href=" + std::regex_replace(dirName, std::regex(" "), "%20")  + term + dirName + "</a><br>";
                    }
                }
                return content;
            }
        } catch (FileNotFoundException) {
            content = this->make404();
        }
    }

    try {
        content = readFile(filePath);
    } catch (FileNotFoundException) {
        content = this->make404();
    }
    return content;
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
