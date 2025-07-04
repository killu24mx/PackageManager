#include "package_manager.h"
#include <iostream>
#include <algorithm>

std::tm time_to_tm(time_t time_s)
{
    std::tm timeInfo{};
    localtime_r(&time_s, &timeInfo);
    return timeInfo;
}

void toLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}



bool PackageManager::loadPackagesInformation()
{
    packages.clear();

    std::ifstream file("/var/lib/dpkg/status");

    if(!file)
    {
        return false;
    }

    std::string packageBlock;
    std::string line;

    while(std::getline(file, line))
    {
        if(line == "")
        {
            Package package = parsePakcageBlock(packageBlock);
            packages.push_back(package);
            packageBlock.clear();
        }
        else
        {
            packageBlock += (line + "\n");
        }
    }

    return true;

}

bool PackageManager::removePackage(const std::string &package_name)
{
    std::string command = "sudo apt purge " + package_name;

    return system(command.c_str());
}

std::tm PackageManager::getPackageInstallationTime(const std::string &package_name)
{
    std::filesystem::path infoPath = "/var/lib/dpkg/info/" + package_name + ".list";

    if(std::filesystem::exists(infoPath))
    {
        struct statx stx{0};

        int res = statx(
            AT_FDCWD, 
            infoPath.string().c_str(), 
            0, 
            STATX_BTIME, 
            &stx
        );

        if(res == 0)
        {
            if (stx.stx_mask & STATX_BTIME) 
            {
                return time_to_tm(stx.stx_btime.tv_sec);
            }
        }
    }


    return {};
}
Package PackageManager::parsePakcageBlock(const std::string &packageBlock)
{
    Package package{};

    std::stringstream ss(packageBlock);

    std::string line;
    
    std::string info;

    auto processLine = [&line, &ss, &info, &package]() ->void
    {
        if(line[0] == ' ')
        {
            if(line != " .")
            {
                info += line;
            }
        }
        else
        {
            if(info.size())
            {
                auto pair = parsePackageParamLine(info);

                if(pair.first == "Package"){
                    package.name = pair.second;
                }
                else if(pair.first == "Description"){
                    package.description = pair.second;
                }
                else if(pair.first == "Version"){
                    package.version = pair.second;
                }
                else if(pair.first == "Installed-Size"){
                    package.size = std::stoul(pair.second);
                }

                package.installation_time = getPackageInstallationTime(package.name);
                package.time_is_valid = static_cast<bool>(package.installation_time.tm_year);
            }

            info = line;
        }
    };

    while(std::getline(ss, line))
    {
        if(!line.size())
        {
            continue;
        }
        processLine();
    }
    processLine();
    

    return package;
}

std::vector<Package> PackageManager::findPackage(std::string name)
{
    std::vector<Package> foundPackages;
    toLower(name);

    for(const auto& package : packages)
    {
        std::string packageName = package.name;
        toLower(packageName);

        if(packageName.find(name) != std::string::npos)
        {
            foundPackages.push_back(package);
        }
    }

    return foundPackages;
}

const std::vector<Package> &PackageManager::getAllPackages()
{
    return packages;
}

std::pair<std::string, std::string> PackageManager::parsePackageParamLine(const std::string &paramLine)
{
    size_t colonPos = paramLine.find(": ");
    return std::make_pair
    (
        paramLine.substr(0, colonPos),
        paramLine.substr(colonPos + 2)
    );
}

std::string tmToString(const std::tm& time, const std::string& format) {
    char buffer[80];
    strftime(buffer, sizeof(buffer), format.c_str(), &time);
    return buffer;
}
