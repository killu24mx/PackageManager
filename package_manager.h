#ifndef _PACKAGE_MANAGER_H_
#define _PACKAGE_MANAGER_H_

#include <string>
#include <vector>
#include <ctime>
#include <filesystem>
#include <sys/stat.h>
#include <fcntl.h>
#include <iomanip>
#include <fstream>

struct Package
{
    std::string name;
    std::string description;
    std::tm installation_time = {0};
    bool time_is_valid = false;
    std::string version;
    size_t size = 0;
};

std::string tmToString(const std::tm& time, const std::string& format = "%Y-%m-%d %H:%M:%S");

class PackageManager
{
public:
    bool loadPackagesInformation();

    bool removePackage(const std::string& package_name);

    std::vector<Package> findPackage(std::string name);

    const std::vector<Package>& getAllPackages();


private:
    std::vector<Package> packages;

    static std::tm getPackageInstallationTime(const std::string& package_name);

    static std::pair<std::string, std::string> parsePackageParamLine(const std::string& paramLine);

    static Package parsePakcageBlock(const std::string& packageBlock);

};


#endif
