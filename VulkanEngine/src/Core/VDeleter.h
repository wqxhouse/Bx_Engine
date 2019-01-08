//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include <vector>

#pragma once
template <typename T>
class VDeleter
{
public:
	VDeleter() : VDeleter([](T, VkAllocationCallbacks*) {}) {}

	VDeleter(
        std::function<void(T, VkAllocationCallbacks*)> deletef)
    {
		this->deleter = [=](T obj) { deletef(obj, nullptr); };
	}

	VDeleter(
        const VDeleter<VkInstance>&                                instance,
        std::function<void(VkInstance, T, VkAllocationCallbacks*)> deletef)
    {
		this->deleter = [&instance, deletef](T obj) { deletef(instance, obj, nullptr); };
	}

	VDeleter(
        const VDeleter<VkDevice>&                                device,
        std::function<void(VkDevice, T, VkAllocationCallbacks*)> deletef)
    {
		this->deleter = [&device, deletef](T obj) { deletef(device, obj, nullptr); };
	}

    VDeleter(
        const VkDevice&                                          device,
        std::function<void(VkDevice, T, VkAllocationCallbacks*)> deletef)
    {
        this->deleter = [device, deletef](T obj) { deletef(device, obj, nullptr); };
    }

	~VDeleter()
    {
		cleanup();
	}

	const T* operator &() const
    {
		return &object;
	}

	T* replace()
    {
		cleanup();
		return &object;
	}

	operator T() const
    {
		return object;
	}

	void operator=(T rhs)
    {
		if (rhs != object)
        {
			cleanup();
			object = rhs;
		}
	}

	template<typename V>
	bool operator==(V rhs)
    {
		return object == T(rhs);
    }

    static std::vector<T> GetRawVector(
        const std::vector<VDeleter<T>>& objList)
    {
        size_t size = objList.size();

        std::vector<T> res(size);

        for (size_t i = 0; i < size; ++i)
        {
            res[i] = objList[i];
        }

        return res;
    }

private:
	T object{ VK_NULL_HANDLE };
	std::function<void(T)> deleter;

	void cleanup()
    {
		if (object != VK_NULL_HANDLE)
        {
			deleter(object);
		}
		object = VK_NULL_HANDLE;
	}
};