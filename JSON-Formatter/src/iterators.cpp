#include "../include/json.hpp"
#include "json.cpp"

//COPIED FROM SLIDES
/* --------------------------------------------------------------------------------------------------------------- */
/* -----------------------------------------------vvv-iterators-vvv----------------------------------------------- */

struct json::list_iterator{
    using iterator_category = std::forward_iterator_tag;
    using value_type = json;
    using pointer = json*;
    using reference = json&;

    list_iterator(impl::list* ptr) : m_ptr(ptr){}

    reference operator*() const{
        return m_ptr->info;
    }

    pointer operator->() const{
        return &(m_ptr->info);
    }

    list_iterator& operator++(){
        m_ptr = m_ptr->next;
        return *this;
    }

    list_iterator operator++(int /* dummy */){
        list_iterator copy = *this;
        m_ptr = m_ptr->next;
        return copy;
    }

    bool operator==(list_iterator const& rhs) const{
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(list_iterator const& rhs) const{
        return !(*this == rhs);
    }

    operator bool() const{
        return m_ptr;   //m_ptr != nullptr
    }

private:
    impl::list* m_ptr;
};

json::list_iterator json::begin_list(){
    if(is_list()){
        return {pimpl->list_head};
    } throw json_exception{"at: list_it begin_list: obj is not a list"};
}

json::list_iterator json::end_list(){
    if(is_list()){
        return {nullptr};
    } throw json_exception{"at: list_it end_list: obj is not a list"};
}



struct json::const_list_iterator{
    using   iterator_category = std::forward_iterator_tag;
    using   value_type = const json;
    using   pointer = json const*;
    using   reference = json const&;

    const_list_iterator(impl::list* ptr) : m_ptr(ptr){}

    reference operator*() const{
        return m_ptr->info;
    }

    pointer operator->() const{
        return &(m_ptr->info);
    }

    const_list_iterator& operator++(){
        m_ptr = m_ptr->next;
        return *this;
    }

    const_list_iterator operator++(int /* dummy */){
        const_list_iterator copy = *this;
        m_ptr = m_ptr->next;
        return copy;
    }

    bool operator==(const_list_iterator const& rhs) const{
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(const_list_iterator const& rhs) const{
        return !(*this == rhs);
    }

    operator bool() const{
        return m_ptr;   //m_ptr != nullptr
    }

private:
    impl::list const* m_ptr;

};

json::const_list_iterator json::begin_list() const{
    if(is_list()){
        return {pimpl->list_head};
    } throw json_exception{"at: list_it begin_list: obj is not a list"};
}

json::const_list_iterator json::end_list() const{
    if(is_list()){
        return {nullptr};
    } throw json_exception{"at: list_it end_list: obj is not a list"};
}



struct json::dictionary_iterator{
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<std::string, json>;
    using pointer = std::pair<std::string, json>*;
    using reference = std::pair<std::string, json>&;

    dictionary_iterator(impl::dict* ptr) : m_ptr(ptr){}

    reference operator*() const{
        return m_ptr->info;
    }

    pointer operator->() const{
        return &(m_ptr->info);
    }

    dictionary_iterator& operator++(){
        m_ptr = m_ptr->next;
        return *this;
    }

    dictionary_iterator operator++(int /* dummy */){
        dictionary_iterator copy = *this;
        m_ptr = m_ptr->next;
        return copy;
    }

    bool operator==(dictionary_iterator const& rhs) const{
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(dictionary_iterator const& rhs) const{
        return !(*this == rhs);
    }

    operator bool() const{
        return m_ptr;   //m_ptr != nullptr
    }

private:
    impl::dict* m_ptr;
};

json::dictionary_iterator json::begin_dictionary(){
    if(is_dictionary()){
        return {pimpl->dict_head};
    } throw json_exception{"at: dict_it begin_dict: obj is not a dict"};
}

json::dictionary_iterator json::end_dictionary(){
    if(is_dictionary()){
        return {nullptr};
    } throw json_exception{"at: dict_it end_dict: obj is not a dict"};
}

struct json::const_dictionary_iterator{
    using iterator_category = std::forward_iterator_tag;
    using value_type = const std::pair<std::string, json>;
    using pointer = std::pair<std::string, json> const*;
    using reference = std::pair<std::string, json> const&;

    const_dictionary_iterator(impl::dict* ptr) : m_ptr(ptr){}

    reference operator*() const{
        return m_ptr->info;
    }

    pointer operator->() const{
        return &(m_ptr->info);
    }

    const_dictionary_iterator& operator++(){
        m_ptr = m_ptr->next;
        return *this;
    }

    const_dictionary_iterator operator++(int /* dummy */){
        const_dictionary_iterator copy = *this;
        m_ptr = m_ptr->next;
        return copy;
    }

    bool operator==(const_dictionary_iterator const& rhs) const{
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(const_dictionary_iterator const& rhs) const{
        return !(*this == rhs);
    }

    operator bool() const{
        return m_ptr;   //m_ptr != nullptr
    }

private:
    impl::dict const* m_ptr;
};

json::const_dictionary_iterator json::begin_dictionary() const{
    if(is_dictionary()){
        return {pimpl->dict_head};
    } throw json_exception{"at: dict_it begin_dict: obj is not a dict"};
}

json::const_dictionary_iterator json::end_dictionary() const{
    if(is_dictionary()){
        return {nullptr};
    } throw json_exception{"at: dict_it end_dict: obj is not a dict"};
}
/* -----------------------------------------------^^^-iterators-^^^----------------------------------------------- */
/* --------------------------------------------------------------------------------------------------------------- */
