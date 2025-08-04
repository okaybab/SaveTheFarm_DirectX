#pragma once
#include <ScriptBehaviour.h>
#include <GameObject.h>
#include <vector>
#include <queue>
#include <memory>

/*//
namespace GOTOEngine
{
    class ObjectPool : public ScriptBehaviour
    {
    private:
        std::vector<std::unique_ptr<GameObject>> m_pool;
        std::queue<int> m_activeQueue;

    public:
    ObjectPool()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
    }

        void Awake()
        {
        }
        void Initialize(int poolSize)
        {
            m_pool.reserve(poolSize);
            for (int i = 0; i < poolSize; ++i) {
                m_pool.push_back(std::make_unique<GameObject>(i));
                m_activeQueue.push(i);
            }
        }

        GameObject* GetPoolObject() {
            if (m_activeQueue.empty()) {
                // isRecycle
                std::cout << "Pool is empty!" << std::endl;
                return nullptr;
            }
            int index = m_activeQueue.front();
            m_activeQueue.pop();

            GameObject* obj = m_pool[index].get();
            obj->SetActive(true);

            return obj;
        }

        void ReturnObject(GameObject* obj) {
            if (obj == nullptr) return;

            obj->SetActive(false);
            m_activeQueue.push(obj->GetInstanceID());
        }
    };
}
//*/