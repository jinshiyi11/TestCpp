#include "IServiceManager.h"
#include <mutex>
#include <map>
#include <cassert>

#define E_NO_ERROR                    0
#define E_ERROR_ALREADY_EXISTS        1
#define E_ERROR_INVALID_PARAMETER     2

namespace easy {

/**
 * IServiceManager全局对象指针
 */
    IServiceManager* gServiceManager = nullptr;

    std::mutex gServiceManagerMutex;/**< 保护gServiceManager对象 */

    class ServiceManager :public IServiceManager
    {
    public:
        virtual int addService(const char* name, IService* service)
        {
            assert(name != nullptr && service != nullptr);
            if (name == nullptr || service == nullptr)
                return E_ERROR_INVALID_PARAMETER;

            std::string serviceName = name;
            if (serviceName.empty())
                return E_ERROR_INVALID_PARAMETER;

            std::lock_guard<std::mutex> lock(mMutex);
            auto it = mServices.find(serviceName);
            if (it != mServices.end()) {
                //I_LOGD(GC("service %s already exist"), name.c_str());
                return E_ERROR_ALREADY_EXISTS;
            }

            mServices.insert(std::make_pair(serviceName, service));
            return E_NO_ERROR;
        }

        virtual IService* getService(const char* name)
        {
            assert(name != nullptr);
            if (name == nullptr)
                return nullptr;

            std::string serviceName = name;
            if (serviceName.empty())
                return nullptr;

            std::lock_guard<std::mutex> lock(mMutex);
            auto it = mServices.find(serviceName);
            if (it != mServices.end()) {
                return it->second;
            } else {
                //I_LOGE(GC("service %s does not exist"), name.c_str());
                return nullptr;
            }
        }

    private:
        std::map<std::string, IService*> mServices;
        std::mutex mMutex;
    };

    IServiceManager* IServiceManager::getInstance()
    {
        if (gServiceManager != nullptr)
            return gServiceManager;

        {
            std::lock_guard<std::mutex> lock(gServiceManagerMutex);
            if (gServiceManager == nullptr) {
                gServiceManager = new ServiceManager();
            }
        }

        return gServiceManager;
    }

    IServiceManager* defaultServiceManager()
    {
        return IServiceManager::getInstance();
    }
}