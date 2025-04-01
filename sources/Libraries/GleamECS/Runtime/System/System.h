#pragma once
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    enum class OrderRelation:uint8_t
    {
        Before,
        After
    };

    class SystemGroup;

    /**
     * 系统是一种高级的封装版事件。
     * 通过相对位置、嵌套等结构，实现在引擎中自由轻松的嵌入各种代码。
     */
    class System
    {
    public:
        static constexpr int32_t MinOrder = std::numeric_limits<int32_t>::lowest();
        static constexpr int32_t MaxOrder = std::numeric_limits<int32_t>::max();
        static constexpr int32_t DefaultOrder = 0;

        static auto GetAllGlobalSystems()
        {
            return allGlobalSystems | std::views::values | std::views::transform(
                [](auto& system) { return std::reference_wrapper<System>(*system); }
            );
        }

        template <typename TSystem> requires std::derived_from<TSystem, System>
        static TSystem& CreateGlobal(std::string_view name = "", const uuids::uuid id = {})
        {
            //注册类型
            Type& systemType = Type::CreateOrGet<TSystem>();
            if (!systemType.GetParent().has_value()) //生成默认父类
                systemType.SetParent(Type::GetType(typeid(System)).value());
            //创建实例
            std::unique_ptr<System> system = std::unique_ptr<System>{static_cast<System*>(systemType.Create())};
            //设置名称
            if (!name.empty())
                system->name = name;
            else if (system->name.empty())
            {
                //生成默认名称
                std::string defaultName = std::string(typeid(TSystem).name());
                defaultName = defaultName.substr(defaultName.find_last_of(' ') + 1);
                system->name = defaultName;
            }
            //设置编号
            if (!id.is_nil())
                system->id = id;
            else if (system->id.is_nil())
                system->id = MD5(name.data()).toArray();
            //注册索引
            assert(!allGlobalSystems.contains(system->id) && "已有相同ID的系统已被注册！");
            auto& result = allGlobalSystems.emplace(system->id, std::move(system)).first->second;

            return *static_cast<TSystem*>(result.get());
        }
        template <typename TSystem> requires std::derived_from<TSystem, System>
        static TSystem Create(std::string_view name)
        {
            TSystem system;

            //设置父类
            Type& systemType = Type::CreateOrGet<TSystem>();
            if (!systemType.GetParent().has_value()) //生成默认父类
                systemType.SetParent(Type::GetType(typeid(System)).value());
            //设置名称
            system.name = name;

            return system;
        }

        static std::optional<std::reference_wrapper<System>> GetGlobalSystem(uuids::uuid id);

        System();
        explicit System(std::optional<std::reference_wrapper<SystemGroup>> group, int minOrder = MinOrder, int maxOrder = MaxOrder, std::string_view name = "");
        System(System& system, OrderRelation orderRelation, std::string_view name = "");
        explicit System(SystemGroup& group);
        System(System&) = delete;
        System& operator=(System&) = delete;
        System(System&&) = default;
        System& operator=(System&&) = default;
        virtual ~System() = default;

        const std::string& GetName() const;
        uuids::uuid GetID() const;
        std::optional<std::reference_wrapper<SystemGroup>> GetGroup() const;
        int GetOrder() const;

        virtual void Start();
        virtual void Update();
        virtual void Stop();

    private:
        friend class SystemEvent;
        Gleam_MakeType_Friend

        inline static std::unordered_map<uuids::uuid, std::unique_ptr<System>> allGlobalSystems = {};

        std::string name;
        uuids::uuid id;
        std::optional<std::reference_wrapper<SystemGroup>> group;
        int minOrder;
        int maxOrder;
        int order;
    };

    Gleam_MakeTypeWithID(System, "")
    {
        Gleam_MakeType_AddField(name);
        Gleam_MakeType_AddField(id);
        Gleam_MakeType_AddField(group);
        Gleam_MakeType_AddField(minOrder);
        Gleam_MakeType_AddField(maxOrder);
        Gleam_MakeType_AddField(order);
    }

#define Gleam_MakeGlobalSystem(systemClass) \
inline systemClass& Global##systemClass = ::Gleam::System::CreateGlobal<systemClass>("",::Gleam::Type::CreateOrGet<systemClass>().GetID());
#define Gleam_MakeGlobalSystemWithID(systemClass,uuidStr) \
inline systemClass& Global##systemClass = ::Gleam::System::CreateGlobal<systemClass>("",uuids::uuid::from_string(uuidStr).value());
}