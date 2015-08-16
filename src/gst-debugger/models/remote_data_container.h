/*
 * remote_data_container.h
 *
 *  Created on: Aug 14, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODELS_REMOTE_DATA_CONTAINER_H_
#define SRC_GST_DEBUGGER_MODELS_REMOTE_DATA_CONTAINER_H_

template<typename T>
class RemoteDataContainer
{
	std::vector<T> container;

	typename std::vector<T>::iterator get_item_it(const std::string &item_name)
	{
		return std::find_if(container.begin(), container.end(), [item_name] (const T& t) {
			return t.get_name() == item_name;
		});
	}

public:
	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::const_iterator const_iterator;

	void update_item(const T &item)
	{
		auto it = get_item_it(item.get_name());
		if (it == container.end())
			container.push_back(item);
		else
			*it = item;
	}

	bool has_item(const std::string &item_name)
	{
		return get_item_it(item_name) != container.end();
	}

	T get_item(const std::string &item_name)
	{
		return *get_item_it(item_name);
	}

	iterator begin() { return container.begin(); }
	const_iterator begin() const { return container.begin(); }
	iterator end() { return container.end(); }
	const_iterator end() const { return container.end(); }

	void remove_item(const std::string &item_name)
	{
		auto it = get_item_it(item_name);
		if (it != container.end())
		{
			container.erase(it);
		}
	}
};

#endif /* SRC_GST_DEBUGGER_MODELS_REMOTE_DATA_CONTAINER_H_ */
