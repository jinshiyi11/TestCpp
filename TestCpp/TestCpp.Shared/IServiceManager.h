#ifndef __ISERVICEMANAGER_H__
#define __ISERVICEMANAGER_H__

#include <string>

namespace easy {

    class IService;
    /**
     * 服务类管理类,注册管理各个模块提供的接口
     */
    class IServiceManager {
    public:
        /**
         * 获取IServiceManager实例
         */
        static IServiceManager* getInstance();

        virtual ~IServiceManager() {}

        /**
         * 添加一个service
         * @param name service名称
         * @param service service对象指针
         * @return 0-成功,否则失败
         */
        virtual int addService(const char* name, IService* service) = 0;

        /**
         * 获取一个service
         * @param name service名称
         * @return
         */
        virtual IService* getService(const char* name) = 0;
    };

    /**
     * 获取一个service并转换成指定类型
     * @param name service名称
     * @return 如果service不存在或不能转换为指定类型返回nullptr
     */
    template<typename T>
    T* getService(const char* name)
    {
        IServiceManager* serviceManager = IServiceManager::getInstance();
        if (serviceManager != nullptr) {
            return dynamic_cast<T*>(serviceManager->getService(name));
        }

        return nullptr;
    }

    /**
     * 方便获取IServiceManager实例，等同于IServiceManager::getInstance()
     */
    IServiceManager* defaultServiceManager();

}
#endif // __ISERVICEMANAGER_H__