#include "json.hpp"

static constexpr double inf = std::numeric_limits<double>::max();

struct json::impl{
    
    std::string str;            //string
    double num;                 //double
    bool statement;             //bool
    bool null;                  //null
//if null is true then statement doesn't matter

    bool is_list;
    bool is_dict;
    struct list{                //list
        json info;
        list* next;
    };

    list* list_head;            //list head
    list* list_tail;


    struct dict{                //dictionary
        std::pair<std::string,json> info;
        dict* next;
    };

    dict* dict_head;            //dictionary head

};

    void LIST(std::istream& lhs, json& rhs);
    void DICT(std::istream& lhs, json& rhs);

    void LIST_PRINT(std::ostream& lhs, json const& rhs);//is required const for the output and input signatures
    void DICT_PRINT(std::ostream& lhs, json const& rhs);

//CHECKED
json::json(){       //allocates a null json-type object
    pimpl = new impl;

    pimpl->str = "";
    pimpl->num = inf;
    pimpl->statement = false;   //value is not relevant since I'll be checking null
    pimpl->null = true;         //if this is true then statement is not relevant
                    // except for is_list and is_dict that must be false to be coherent
    pimpl->is_list = false;
    pimpl->is_dict = false;
    pimpl->list_head = nullptr;
    pimpl->dict_head = nullptr;
    pimpl->list_tail = nullptr;
}

//CHECKED
json::json(json const& j){          //copy constructor
    pimpl = new impl;

    pimpl->str = j.pimpl->str;
    pimpl->num = j.pimpl->num;
    pimpl->statement = j.pimpl->statement;
    pimpl->null = j.pimpl->null;

    pimpl->is_list = false;
    pimpl->is_dict = false;
    pimpl->list_head = nullptr;
    pimpl->dict_head = nullptr;
    pimpl->list_tail = nullptr;

    if(j.is_list()){
        pimpl->is_list = true;
        impl::list* temp = j.pimpl->list_head;
        while(temp != nullptr){
            push_back(temp->info);
            temp = temp->next;
        }
    }else{
        if(j.is_dictionary()){
            pimpl->is_dict = true;
            impl::dict* temp = j.pimpl->dict_head;
            while(temp != nullptr){
                insert(temp->info);
                temp = temp->next;
            }
        }
    }
}

//CHECKED 
//(try moving the just the impl* (pimpl) without touching anything)
json::json(json&& j){       //move constructor
    if(this != &j){
        pimpl = j.pimpl;
        j.pimpl = nullptr;
        delete j.pimpl;
    }

}

//IF SET_NULL WORKS THEN IT'S OK
json::~json(){
    set_null();
    delete pimpl;
}

//CHECKED
json& json::operator=(json const& j){
    if(this != &j){        //if this an j are not the same obj
        set_null();        //clear this

        if(j.is_list()){
            pimpl->is_list = true;
            impl::list* temp = j.pimpl->list_head;
            while(temp != nullptr){
                push_back(temp->info);
                temp = temp->next;
            }
        }else{
            if(j.is_dictionary()){
                pimpl->is_dict = true;
                impl::dict* temp = j.pimpl->dict_head;
                while(temp != nullptr){
                    insert(temp->info);
                    temp = temp->next;
                }
            }
        }
        pimpl->statement = j.pimpl->statement;
        pimpl->num = j.pimpl->num;
        pimpl->null = j.pimpl->null;
        pimpl->str = j.pimpl->str;
    }
    return *this;
}

json& json::operator=(json&& j){
    if(this != &j){
        set_null();

        pimpl->list_head = j.pimpl->list_head;
        pimpl->dict_head = j.pimpl->dict_head;
        pimpl->list_tail = j.pimpl->list_tail;

        pimpl->is_list = j.pimpl->is_list;
        pimpl->is_dict = j.pimpl->is_dict;

        pimpl->num = j.pimpl->num;
        pimpl->str = j.pimpl->str;
        pimpl->statement = j.pimpl->statement;
        pimpl->null = j.pimpl->null;
        j.set_null();
    }
    return *this;
}

bool json::is_list() const{
    bool check =(!(pimpl->null) and (pimpl->str == "") and (pimpl->num == inf) 
    and (pimpl->is_list) and !(pimpl->is_dict));
    return check;
}

bool json::is_dictionary() const{
    bool check = (!(pimpl->null) and (pimpl->str == "") and (pimpl->num == inf) 
    and !(pimpl->is_list) and (pimpl->is_dict));
    return check;
}

bool json::is_string() const{
    bool check = (!(pimpl->null) and (pimpl->str != "") and (pimpl->num == inf) 
    and !(pimpl->is_list) and !(pimpl->is_dict));
    return check;
}

bool json::is_number() const{
    bool check = (!(pimpl->null) and (pimpl->str == "") and (pimpl->num != inf) 
    and !(pimpl->is_list) and !(pimpl->is_dict));
    return check;
}

bool json::is_bool() const{
    bool check = (!(pimpl->null) and (pimpl->str == "") and (pimpl->num == inf) 
    and !(pimpl->is_list) and !(pimpl->is_dict));
    return check;
}

bool json::is_null() const{
    bool check = (pimpl->null and (pimpl->str == "") and (pimpl->num == inf) 
    and !(pimpl->is_list) and !(pimpl->is_dict));
    return check;
}

json const& json::operator[](std::string const& key) const{
    if(!is_dictionary()){
        throw json_exception{"at: op[]const: obj is not a dict"};
    }else{
        impl::pd ptr = pimpl->dict_head;
        while(ptr != nullptr){
            if(ptr->info.first == key){
                return ptr->info.second; 
            }
            ptr = ptr->next;
        }
        throw json_exception{"at: op[]const: key is not in dict"};
    }
}

json& json::operator[](std::string const& key){
    if(!is_dictionary()){
        throw json_exception{"at: op[]const: obj is not a dict"};
    }else{
        impl::pd ptr = pimpl->dict_head;
        while(ptr != nullptr){
            if(ptr->info.first == key){
                return ptr->info.second; 
            }
            ptr = ptr->next;
        }
        json nuovo;
        insert(std::make_pair(key, nuovo));
        return ptr->next->info.second;
    }
}

/* --------------------------------------------------------------------------------------------------------------- */
/* -----------------------------------------------vvv-iterators-vvv----------------------------------------------- */

struct json::list_iterator{
    using iterator_category = std::forward_iterator_tag;
    using value_type = json;
    using pointer = json*;
    using reference = json&;

    list_iterator(json::impl::list* ptr) : m_ptr(ptr) {}

    reference operator*() const{
        return m_ptr->info;
    }

    pointer operator->() const{
        return &m_ptr->info;
    }

    list_iterator& operator++(){
        m_ptr = m_ptr->next;
        return *this;
    }

    list_iterator operator++(int dummy){
        list_iterator temp = *this;
        ++(*this);
        return temp;
    }

    bool operator==(list_iterator const& rhs) const{
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(list_iterator const& rhs) const{
        return !(*this == rhs);
    }

private:
    json::impl::list* m_ptr;
};

json::list_iterator json::begin_list(){     //ritorna un iteratore all'inizio di una lista
    if(is_list()){
        json::list_iterator it(this->pimpl->list_head);
        return it;
    }else{
        throw json_exception{"at: begin_list: json is not a list"};
    }
}

json::list_iterator json::end_list(){
    if(is_list()){
        json::list_iterator it(nullptr);
        return it;
    }else{
        throw json_exception{"at: end_list: json is not a list"};
    }
}

struct json::dictionary_iterator{
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<std::string, json>;
    using pointer = std::pair<std::string, json>*;
    using reference = std::pair<std::string, json>&;

    dictionary_iterator(json::impl::dict* ptr) : m_ptr(ptr) {}

    reference operator*() const{
        return m_ptr->info;
    }

    pointer operator->() const{
        return &m_ptr->info;
    }

    dictionary_iterator& operator++(){
        m_ptr = m_ptr->next;
        return *this;
    }

    dictionary_iterator operator++(int dummy){
        dictionary_iterator temp = *this;
        ++(*this);
        return temp;
    }

    bool operator==(dictionary_iterator const& rhs) const{
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(dictionary_iterator const& rhs) const{
        return !(*this == rhs);
    }

    private:
        json::impl::dict* m_ptr; 
};

json::dictionary_iterator json::begin_dictionary(){     //ritorna un iteratore all'inizio di una lista
    if(is_dictionary()){
        json::dictionary_iterator it(this->pimpl->dict_head);
        return it;
    }else{
        throw json_exception{"at: begin_dict: json is not a dict"};
    }
}

json::dictionary_iterator json::end_dictionary(){
    if(is_dictionary()){
        json::dictionary_iterator it(nullptr);
        return it;
    }else{
        throw json_exception{"at: end_dict: json is not a dict"};
    }
}


struct json::const_list_iterator{
    using iterator_category = std::forward_iterator_tag;
    using value_type = const json;
    using pointer = json const*;
    using reference = json const&;

    const_list_iterator(json::impl::list* ptr) : m_ptr(ptr) {}

    reference operator*() const{
        return m_ptr->info;
    }

    pointer operator->() const{
        return &m_ptr->info;
    }

    const_list_iterator& operator++(){
        const_list_iterator temp = m_ptr->next;
        return temp;
    }
/*
    const_list_iterator operator++(int dummy){          //does it make sense in a const iterator?
        const_list_iterator temp = *this;
        ++(*this);
        return temp;
    }
*/
    bool operator==(const_list_iterator const& rhs) const{
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(const_list_iterator const& rhs) const{
        return !(*this == rhs);
    }


    private:
        json::impl::list* const m_ptr;
};

json::const_list_iterator json::begin_list() const{     //ritorna un iteratore all'inizio di una lista
    if(is_list()){
        json::const_list_iterator it(this->pimpl->list_head);
        return it;
    }else{
        throw json_exception{"at: begin_list_const: json is not a list"};
    }
}

json::const_list_iterator json::end_list() const{
    if(is_list()){
        json::const_list_iterator it(nullptr);
        return it;
    }else{
        throw json_exception{"at: end_list_const: json is not a list"};
    }
}


struct json::const_dictionary_iterator{
    using iterator_category = std::forward_iterator_tag;
    using value_type = const std::pair<std::string, json>;
    using pointer = std::pair<std::string, json> const*;
    using reference = std::pair<std::string, json> const&;

    const_dictionary_iterator(json::impl::dict* ptr) : m_ptr(ptr) {}

    reference operator*() const{
        return m_ptr->info;
    }

    pointer operator->() const{
        return &m_ptr->info;
    }

    const_dictionary_iterator& operator++(){
        const_dictionary_iterator temp = m_ptr->next;
        return temp;
    }
/*

    const_dictionary_iterator operator++(int dummy){             //does it make sense in a const iterator?
        const_dictionary_iterator temp = *this;
        ++(*this);
        return temp;
    }
*/
    bool operator==(const_dictionary_iterator const& rhs) const{
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(const_dictionary_iterator const& rhs) const{
        return !(*this == rhs);
    }

    private:
        json::impl::dict* const m_ptr;
};

json::const_dictionary_iterator json::begin_dictionary() const{     //ritorna un iteratore all'inizio di una lista
    if(is_dictionary()){
        json::const_dictionary_iterator it(this->pimpl->dict_head);
        return it;
    }else{
        throw json_exception{"at: begin_dict_const: json is not a dict"};
    }
}

json::const_dictionary_iterator json::end_dictionary() const{
    if(is_dictionary()){
        json::const_dictionary_iterator it(nullptr);
        return it;
    }else{
        throw json_exception{"at: begin_dict_const: json is not a dict"};
    }
}

/* -----------------------------------------------^^^-iterators-^^^----------------------------------------------- */
/* --------------------------------------------------------------------------------------------------------------- */

double& json::get_number(){
    if(is_number()){
        return pimpl->num;
    }else{
        throw json_exception{"at: get_number: obj is not a number"};
    }
}

double const& json::get_number() const{
    if(is_number()){
        return pimpl->num;
    }else{
        throw json_exception{"at: get_number_const: obj is not a number"};
    }
}

bool& json::get_bool(){
    if(is_bool()){
        return pimpl->statement;
    }else{
        throw json_exception{"at: get_bool: obj is not a bool"};
    }
}

bool const& json::get_bool() const{
    if(is_bool()){
        return pimpl->statement;
    }else{
        throw json_exception{"at: get_bool_const: obj is not a bool"};
    }
}

std::string& json::get_string(){
    if(is_string()){
        return pimpl->str;
    }else{
        throw json_exception{"at: get_string: obj is not a string"};
    }
}

std::string const& json::get_string() const{
    if(is_string()){
        return pimpl->str;
    }else{
        throw json_exception{"at: get_string_const: obj is not a string"};
    }
}

void json::set_string(std::string const& s){
    if(is_string()){
        pimpl->str = s;
    }else{
        set_null();
        pimpl->null = false;
        pimpl->str = s;
    }
}

void json::set_bool(bool b){
    if(is_bool()){
        pimpl->statement = b;
    }else{
        set_null();
        pimpl->null = false;
        pimpl->statement = b;
    }
}

void json::set_number(double x){
    if(is_number()){
        pimpl->num = x;
    }else{
        set_null();
        pimpl->null = false;
        pimpl->num = x;
    }
}

void json::set_null(){
    //clears an impl
    pimpl->str = "";
    pimpl->num = inf;
    pimpl->null = true;
    pimpl->is_list = false;
    pimpl->is_dict = false;

    while(pimpl->list_head != nullptr){      //deallocate list
        impl::pl temp = pimpl->list_head->next;
        delete pimpl->list_head;
        pimpl->list_head = temp;
    }
    pimpl->list_tail = nullptr;
    pimpl->list_head = nullptr;
    while(pimpl->dict_head != nullptr){      //deallocate dictionary
        impl::pd temp = pimpl->dict_head->next;
        delete pimpl->dict_head;
        pimpl->dict_head = temp;
    }
    pimpl->dict_head = nullptr;
}

void json::set_list(){
    set_null();
    pimpl->is_list = true;
    pimpl->null = false;
}

void json::set_dictionary(){
    set_null();
    pimpl->is_dict = true;
    pimpl->null = false;
}

void json::push_front(json const& x) {
    if (is_list()) {
        if (pimpl->list_head == nullptr) { // If the list is empty
            pimpl->list_head = new impl::list;
            pimpl->list_head->info = x;
            pimpl->list_head->next = nullptr;
            pimpl->list_tail = pimpl->list_head;
        } else { // If the list is NOT empty
            impl::list* temp = new impl::list; // Corrected declaration
            temp->info = x;
            temp->next = pimpl->list_head;
            pimpl->list_head = temp;
        }
    } else {
        throw json_exception{"at: push_front: obj is not a list"};
    }
}

void json::push_back(json const& x) {
    if (is_list()) {
        if (pimpl->list_head == nullptr) {
            pimpl->list_head = new impl::list;
            pimpl->list_head->info = x;
            pimpl->list_head->next = nullptr;
            pimpl->list_tail = pimpl->list_head;
        } else {
            impl::list* newNode = new impl::list;
            newNode->info = x;
            newNode->next = nullptr;
            pimpl->list_tail->next = newNode; // Connect the new node to the end of the list
            pimpl->list_tail = newNode; // Update list_tail to point to the new node
        }
    } else {
        throw json_exception{"at: push_back: obj is not a list"};
    }
}


void json::insert(std::pair<std::string, json> const& x){
    if(is_dictionary()){
        if(pimpl->dict_head == nullptr){
            pimpl->dict_head = new impl::dict;
            pimpl->dict_head->info = x;
            pimpl->dict_head->next = nullptr;
        }else{
            impl::dict* temp = new impl::dict;
            temp->info = x;
            temp->next = pimpl->dict_head;
            pimpl->dict_head = temp;
        }
    }else{
        throw json_exception{"at: insert: obj is not a dict"};
    }
}

void LIST(std::istream& lhs, json& rhs){    //reads a LIST from lhs and puts it in rhs
    char c;
    lhs >> c;       //reads the first char to see what it needs to parse then
    std::cout << "______PARSING:______ " << c << std::endl;
    if(c == '"'){   //parses a string, without considering spaces and tabs
        std::string s;
        do{
            if(c == 92){
                lhs >> c;       //if there's an escape, consume another char to skip the escape
            }
            s += c;
            lhs >> c;
        }while(c != '"');
        s += c;
        json element;
        element.set_string(s);
            assert(element.is_string());
        rhs.push_back(element);
        if(s.back() == ','){
            lhs.putback(s.back());
        }
        std::cout << "is a string: " << s << std::endl;
    }else{
        if(c >= 48 && c <= 57){ //parse a double
            double num;
            lhs.putback(c);
            lhs >> num;
            json element;
            element.set_number(num);
                assert(element.is_number());
            rhs.push_back(element);
            std::cout << "is a number: " << num << std::endl;
        }else{
            if(c == 't' ||  c == 'f'){  //parse a boolean
                std::string s;
                lhs.putback(c);
                lhs >> s;
                json element;
                if(s == "true" || s == "true," || s == "true]" || s == "true],"){ //add true]]
                    element.set_bool(true);
                }else{
                    if(s == "false" || s == "false," || s == "false]" || "false],"){
                        element.set_bool(false);
                    }else{              //not true nor false
                        std::cout << std::endl << "value: " << s << std::endl;
                        throw json_exception{"at: LIST input: obj read is neither true or false"};
                    }
                }
                    assert(element.is_bool());
                rhs.push_back(element);
                if(s.back() == ',' || s.back() == ']'){
                    lhs.putback(s.back());
                }
                
                std::cout << "is a bool:" << s << std::endl;
            }else{
                if(c == 'n'){   //parse a NULL
                    std::string s;
                    lhs.putback(c);

                    lhs >> s;
                    if(s == "null" || s == "null," || s == "null]" || s == "null],"){
                        json element;
                        element.set_null();
                            assert(element.is_null());
                        rhs.push_back(element);
                    }else{
                        throw json_exception{"at: LIST input: obj read is not a null"};
                    }
                    if(s.back() == ',' || s.back() == ']'){
                        lhs.putback(s.back());
                    }
                    
                    std::cout << "is a null: " << s << std::endl;
                }else{
                    if(c == '['){   //parse a list
                        json element;
                        element.set_list();
                            assert(element.is_list());
                        LIST(lhs, element);    //reads the next list from input and puts it into element
                        rhs.push_back(element);
                        std::cout << "is a list" << std::endl;
                    }else{
                        if(c == '{'){   //parse a dictionary
                            json element;
                            element.set_dictionary();
                                assert(element.is_dictionary());
                            DICT(lhs, element);
                            rhs.push_back(element);
                            std::cout << "is a dict" << std::endl;
                        }
                    }
                }
            }
        }
    }
    lhs >> c;
    std::cout << "arrived at the end of LIST with value: " << c << std::endl;

    if(c == ']'){
        std::cout << "next is ]" << std::endl;
        return;
    }else{                      //this is kind of the recursive call
        if(c == ','){
            std::cout << "next is ," << std::endl;
            LIST(lhs, rhs);
        }
    }               //-------------------------------------------------------
    std::cout << "done" << std::endl;
}

//LIST input is done, i have to finish DICT input and then see how to use the possible iterators to output
//everything

void DICT(std::istream& lhs, json& rhs){    //reads a DICT from lhs and puts it in rhs
    char c;
    std::string s;
    lhs >> c;   //reads  the "
    do{
        if(c == 92){
            lhs >> c;       //if there's an escape consume another char
        }
        s += c;
        lhs >> c; 
    }while(c != '"');
    //now s should be the key, I have to parse the correspondent JSON
    lhs >> c;
    if(c != ':'){
        std::cout << c << "             ";
        throw json_exception{"at: DICT input: char is not \":\""};
    }
    lhs >> c;
    if(c == '"'){   //parses a string, without considering spaces and tabs
        std::string str;
        do{
            if(c == 92){
                lhs >> c;       //if there's an escape, consume another char to skip the escape
            }
            str += c;
            lhs >> c;
        }while(c != '"');
        str += c;
        json element;
        element.set_string(str);
        std::pair<std::string, json> info{s, element};
        rhs.insert(info);
        std::cout << "is a string: " << str << std::endl;
    }else{
        if(c >= 48 && c <= 57){
            lhs.putback(c);
            double num;
            lhs >> num;
            json element;
            element.set_number(num);
            std::pair<std::string, json> info{s, element};
            rhs.insert(info);
            std::cout << "is a number: " << num << std::endl;
        }else{
            if(c == 't' ||  c == 'f'){  //parse a boolean
                std::string str;
                lhs.putback(c);

                lhs >> str;
                json element;
                if(str == "true" || str == "true," || str == "true}" || str == "true},"){
                    element.set_bool(true);
                }else{
                    if(str == "false" || str == "false," || str == "false}" || str == "false},"){
                        element.set_bool(false);
                    }else{              //not true nor false
                        throw json_exception{"at: LIST input: obj read is neither true or false"};
                    }
                }
                std::pair<std::string, json> info{s, element};
                rhs.insert(info);
                if(str.back() == ',' || str.back() == '}'){
                    lhs.putback(str.back());
                }
                if(str.back() == ',' || str.back() == '}'){
                    lhs.putback(str.back());
                }
                std::cout << "is a bool: " << str << std::endl;
            }else{
                if(c == 'n'){   //parse a NULL
                    std::string str;
                    lhs.putback(c);

                    lhs >> str;
                    if(str == "null" || str == "null," || str == "null}" || str == "null},"){
                        json element;
                        element.set_null();
                        std::pair<std::string, json> info{s, element};
                        rhs.insert(info);
                    }else{
                        throw json_exception{"at: LIST input: obj read is not a null"};
                    }
                    if(str.back() == ',' || str.back() == '}'){
                        lhs.putback(s.back());
                    }
                    if(str.back() == ',' || str.back() == '}'){
                        lhs.putback(s.back());
                    }
                    std::cout << "is a null: " << str << std::endl;
                }else{
                    if(c == '['){
                        //parse list
                        json element;
                        element.set_list();
                        LIST(lhs, element); //populates element with the list in lhs
                        std::pair<std::string, json> info{s, element};
                        rhs.insert(info);
                        std::cout << "is a LIST" << std::endl;
                    }else{
                        if(c == '{'){
                            //parse dictionary
                            json element;
                            element.set_dictionary();
                            DICT(lhs, element); //populates element with the dictionary in lhs
                            std::pair<std::string, json> info{s, element};
                            rhs.insert(info);
                            std::cout << "is a DICT" << std::endl;
                        }
                    }
                }
            }
        }
    }
    lhs >> c;
    if(c == '}'){
        return;
    }else{                      //this is kind of the recursive call
        if(c == ','){
            DICT(lhs, rhs);
        }
    }  
}

void LIST_PRINT(std::ostream& lhs, json const& rhs){
    auto it = rhs.begin_list();
    while(it != rhs.end_list()){
        lhs << *it;     //right here *it is a json so it will recursively call the output operator
        ++it;
        if(it != rhs.end_list()){
            lhs << ",";
        }
        lhs << std::endl;
    }
}

void DICT_PRINT(std::ostream& lhs, json const& rhs){
    auto it = rhs.begin_dictionary();
    while(it != rhs.end_dictionary()){
        lhs << it->first;
        lhs << "\" : ";
        lhs << it->second;
        ++it;
        if(it != rhs.end_dictionary()){
            lhs << ",";
        }
        lhs << std::endl;
    }
}



std::ostream& operator<<(std::ostream& lhs, json const& rhs){   //takes inputs from rhs and puts them into lhs (lhs << rhs)

    if(rhs.is_bool()){
            assert(rhs.is_bool());
        bool bin;
        bin = rhs.get_bool();
        if(bin == true){
            lhs << "true";
        }
        if(bin == false){
            lhs << "false";
        }
    }else{
        if(rhs.is_null()){
                assert(rhs.is_null());
            lhs << "null";
        }else{
            if(rhs.is_number()){
                    assert(rhs.is_number());
                lhs << rhs.get_number();
            }else{
                if(rhs.is_string()){
                        assert(rhs.is_string());
                    lhs << rhs.get_string();
                }else{
                    if(rhs.is_list()){
                            assert(rhs.is_list());
                        lhs << "[" << std::endl;
                        LIST_PRINT(lhs, rhs);
                        lhs << "]" << std::endl;
                    }else{
                        if(rhs.is_dictionary()){
                                assert(rhs.is_dictionary());
                            lhs << "{" << std::endl;
                            DICT_PRINT(lhs, rhs);
                            lhs << "}" << std::endl;
                        }
                    }
                }
            }
        }
    }
    
    return lhs;
}

std::istream& operator>>(std::istream& lhs, json& rhs){ //takes inputs from lhs >> and puts them into rhs (lhs >> rhs)
    char c;
    std::cout << "entered operator <<" << std::endl;
    lhs >> c;
    if(c == '['){   //is a list
        rhs.set_list(); //creates an empty list, i need to add the members with push_back, this is handled by LIST()
        std::cout << "is a LIST" << std::endl;
        LIST(lhs, rhs); //must read a list from std::input and put it in rhs
    }else{
        if(c == '{'){   //is a dictionary
            rhs.set_dictionary();
            std::cout << "is a DICT" << std::endl;
            DICT(lhs, rhs); //must read a dictionary from std::input and put it in rhs
        }    
    }

    
    return lhs;
}


int main(){
    
    json test;
    try{
        std::cin >> test;
    }
    catch(json_exception error){
        std::cout << std::endl << "-----------------------------------------"
        << std::endl << error.msg << std::endl << "-----------------------------------------" << std::endl;
    }
    try{
        std::cout << test;
    }
    catch(json_exception error){
        std::cout << std::endl << "-----------------------------------------"
        << std::endl << error.msg << std::endl << "-----------------------------------------" << std::endl;
    }
   
    return 0;
}





/*iterators don't work, the const iterators can't be incremented so they never
point to the next item in the list or dictionary*/

/* change every instance of "pl" into "list*" and pd to "dict*" */