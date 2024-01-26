






namespace what {
template<typename T>
struct promise_manager {
    using filter_function = std::function<bool(const T&)>;
    template<typename K>
    using promise_function = std::function<K(const T&)>;


    struct promise_callback {
        uint32_t m_message_number;
        std::function<void(const T&)> handle = [](const T&) {};
        promise_callback(uint32_t p_message_number) : m_message_number(p_message_number) {};
    };

template <typename K> 
class promise {
    private:
        promise_function<K> m_callback;
        promise_manager * m_manager;
        typename hal::static_list<promise_callback>::item m_promise_callback;
        K m_value;
        bool m_has_value = false;
    public:
        K wait_for() {
            while(!m_has_value) {}
            return m_value;
        }
        K value() {
            return m_value;
        }
        bool has_value() {
            return m_has_value;
        }


        void operator()(const T& value) {
            m_value = m_callback(value);
            m_has_value = true;
        }

        promise(promise_manager * p_manager, promise_function<K> p_callback) : 
            m_callback(p_callback), 
            m_manager(p_manager), 
            m_promise_callback(p_manager->m_promises.push_back(promise_callback(p_manager->get_next_message_number()))) {
            m_promise_callback.get().handle = std::ref(*this);
        }
};

    hal::static_list<promise_callback> m_promises;
    
    template<typename K>
    promise<K> create_promise(promise_function<K> callback) {
        return promise<K>(this, callback);
    }

    uint32_t current_message_number = 0;
    uint32_t next_promised_message_number = 1;

    uint32_t get_next_message_number() {
        if(current_message_number >= next_promised_message_number) {
            next_promised_message_number = current_message_number + 1;
            // return next_promised_message_number ++;
        }
        return next_promised_message_number ++;
    }

    void process(const T& value) {
        current_message_number++;
        for(auto& handle : m_promises) {
            if(handle.m_message_number == current_message_number) {
                handle.handle(value);
                return;
            }
        }
    }
};
};