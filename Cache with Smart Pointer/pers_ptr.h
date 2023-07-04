#include "file_access.h"
#include <list>
#include <unordered_map>
#include <memory>

class EvictionOldestUnlock {
public:
	using hint_type = std::list<internal_ptr>::iterator;

	void load(internal_ptr ip) {
		list.push_front(ip);
		hintMap[ip] = list.begin();
	}

	hint_type unlock(internal_ptr ip) {
		hint_type it = hintMap[ip];
		list.splice(list.begin(), list, it);
		return it;
		
	}

	void relock(internal_ptr ip, hint_type hint) {
		list.splice(list.begin(), list, hint);
	}

	internal_ptr release() {
		internal_ptr lru = list.back();
		list.pop_back();
		hintMap.erase(lru);
		return lru;
	}

private:
	std::list<internal_ptr> list;
	std::unordered_map<internal_ptr, hint_type> hintMap;
};
template < typename ValueType, size_t arity, typename EvictionPolicy>
class pers_ptr;

template <typename ValueType, size_t arity, typename EvictionPolicy>
class cached_container {
public:
	cached_container(size_t c, file_descriptor f) : fd(f), cap(c) {}


	pers_ptr<ValueType, arity, EvictionPolicy> load_and_return(internal_ptr ip) {

		auto it = cmap.find(ip);

		if (ip == null_internal_ptr) {
			return pers_ptr<ValueType, arity, EvictionPolicy>(nullptr, null_internal_ptr, this);
		}


		if (it == cmap.end()) {
			if (cmap.size() >= cap) {
				auto released_ip = policy.release();
				cmap.erase(cmap.find(released_ip));
				unlockedMap.erase(unlockedMap.find(released_ip));
			}
			auto node = getNode(ip);
			policy.load(ip);
			cmap[ip] = std::make_shared<persistent_node<ValueType, arity>>(node);

			return pers_ptr<ValueType, arity, EvictionPolicy>(cmap[ip], ip, this);
		}

		else {
			auto unlocked_it = unlockedMap.find(ip);
			if (unlocked_it != unlockedMap.end()) {
				typename EvictionPolicy::hint_type hint = unlockedMap[ip];
				policy.relock(ip, hint);
				unlockedMap.erase(unlocked_it);
			}
		}
		return pers_ptr<ValueType, arity, EvictionPolicy>(it->second, ip, this);
	}

	persistent_node<ValueType, arity> getNode(internal_ptr ip) {
		persistent_node<ValueType, arity> node;
		file_access<persistent_node<ValueType, arity>>().read(fd, ip, node);
		return node;
	}

	pers_ptr<ValueType, arity, EvictionPolicy> root_ptr() {
		return load_and_return(root_internal_ptr);
		
	}
	pers_ptr<ValueType, arity, EvictionPolicy> null_ptr() {
		return load_and_return(null_internal_ptr);
	}


	void unlock(internal_ptr ip) {

		if (ip != null_internal_ptr) {
			auto it = cmap.find(ip);
			if (it != cmap.end())
			{
				if ((it->second).use_count() <= 2) {

					typename EvictionPolicy::hint_type hint = policy.unlock(ip);
					unlockedMap[ip] = hint;
				}
			}
		}
	}

private:

	file_descriptor fd;
	size_t cap;
	EvictionPolicy policy;
	std::unordered_map<internal_ptr, std::shared_ptr<persistent_node<ValueType, arity>>> cmap;
	std::unordered_map<internal_ptr, typename EvictionPolicy::hint_type> unlockedMap;
};

template < typename ValueType, size_t arity, typename EvictionPolicy>
class pers_ptr {
public:

	using s_ptr_node = std::shared_ptr < persistent_node < ValueType, arity>>;
	pers_ptr(s_ptr_node shar_ptr, internal_ptr i, cached_container<ValueType, arity, EvictionPolicy>* c) :
		ip(i), cc(c), shared_ptr_to_node(shar_ptr) { }


	pers_ptr(const pers_ptr&) = delete;
	pers_ptr& operator=(const pers_ptr&) = delete;

	pers_ptr(pers_ptr&& other) noexcept : ip(other.ip), cc(other.cc), shared_ptr_to_node(other.shared_ptr_to_node) {
		other.ip = null_internal_ptr, other.cc = nullptr, other.shared_ptr_to_node = nullptr;
	}
	

	~pers_ptr() {
		if (ip != null_internal_ptr) {
			cc->unlock(ip);
		}
	}

	pers_ptr& operator=(pers_ptr&& other) noexcept {
		if (this != &other) {
			if (ip != null_internal_ptr) {
				cc->unlock(ip);
			}
			ip = other.ip;
			cc = other.cc;
			shared_ptr_to_node = other.shared_ptr_to_node;
		}
		return *this;
	}
	explicit operator bool() const noexcept {
		return ip != null_internal_ptr;
	}
	pers_ptr get_ptr(size_t index) {
		internal_ptr edge = shared_ptr_to_node->ptr[index];
		return cc->load_and_return(edge);
	}
	const ValueType& operator*() {
		return shared_ptr_to_node->value;
	}
	const ValueType* operator->() {
		return *(shared_ptr_to_node->value);
	}

	internal_ptr ip;
	cached_container<ValueType, arity, EvictionPolicy>* cc;
	std::shared_ptr < persistent_node < ValueType, arity>> shared_ptr_to_node;
private:
};