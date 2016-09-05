#ifndef __ISERVICE_H__
#define __ISERVICE_H__

namespace easy {
/**
 * 服务类基类，服务类需实现该接口
 */
    class IService
    {
    public:
        virtual ~IService() {}
    };

    /**
     * 辅助类模板，方便将一个类服务化
     * 如：IServiceManager::getInstance()->addService("servicename",new IServiceable<A>());
     * 将类A服务化
     */
    template<typename T>
    class IServiceable :public T, public IService
    {

    };

}
#endif // __ISERVICE_H__