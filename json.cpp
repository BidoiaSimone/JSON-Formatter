#include "json.hpp"

static constexpr double inf = std::numeric_limits<double>::max();

struct json::impl{
    
    std::string str;            //string
    double num;                 //double
    bool statement;             //bool
    bool null;                  //null
//if null is true then statement doesn't matter

    bool is_list;           //these are required because the list or dict could exist but be empty
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

    void LIST_PARSER(std::istream& lhs, json& rhs);
    void DICT_PARSER(std::istream& lhs, json& rhs);
    void BOOLEAN_PARSER(std::istream& lhs, json& rhs);
    void NUMBER_PARSER(std::istream& lhs, json& rhs);
    void NULL_PARSER(std::istream& lhs, json& rhs);
    void STRING_PARSER(std::istream& lhs, json& rhs);

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

//CHECKED   
json& json::operator=(json&& j){            //move operator
    if(this != &j){     //if they are not the same obj
        set_null();     //clear this object

        pimpl = j.pimpl;
        j.pimpl = nullptr;
        delete j.pimpl;
    }
    return *this;
}

//CHECKED
bool json::is_list() const{
    
    bool check;
    check = pimpl->is_list      && 
            !(pimpl->is_dict)   &&
            !(pimpl->null)      &&
            pimpl->num == inf   &&
            pimpl->str == "";    
    return check;
}
//CHECKED
bool json::is_dictionary() const{
    bool check;
    check = !(pimpl->is_list)   &&
            pimpl->is_dict      &&
            !(pimpl->null)      &&
            pimpl->num == inf   &&
            pimpl->str == "";
    return check;
}
//CHECKED
bool json::is_string() const{
    bool check;
    check = !(pimpl->is_list)   &&
            !(pimpl->is_dict)   &&
            !(pimpl->null)      &&
            pimpl->num == inf   &&
            pimpl->str != "";
    return check;
}
//CHECKED
bool json::is_number() const{
    bool check;
    check = !(pimpl->is_list)   &&
            !(pimpl->is_dict)   &&
            !(pimpl->null)      &&
            pimpl->num != inf   &&
            pimpl->str == "";
    return check;
}
//CHECKED
bool json::is_bool() const{
    bool check;
    check = !(pimpl->is_list)   &&
            !(pimpl->is_dict)   &&
            !(pimpl->null)      &&
            pimpl->num == inf   &&
            pimpl->str == "";
    return check;
}
//CHECKED
bool json::is_null() const{
    bool check;
    check = !(pimpl->is_list)   &&
            !(pimpl->is_dict)   &&
            pimpl->null         &&
            pimpl->num == inf   &&
            pimpl->str == "";
    return check;
}

//CHECKED
json const& json::operator[](std::string const& key) const{
    if(!is_dictionary()){
        throw json_exception{"at: op[]const: obj is not a dict"};
    }else{
        impl::dict* ptr = pimpl->dict_head;
        while(ptr != nullptr){
            if(ptr->info.first == key){
                return ptr->info.second; 
            }
            ptr = ptr->next;
        }
        throw json_exception{"at: op[]const: key is not in dict"};
    }
}

//CHECKED
json& json::operator[](std::string const& key){
    if(!is_dictionary()){
        throw json_exception{"at: op[]const: obj is not a dict"};
    }else{
        impl::dict* ptr = pimpl->dict_head;
        while(ptr != nullptr){
            if(ptr->info.first == key){
                return ptr->info.second;//if it finds the key it returns the value
            }
            ptr = ptr->next;
        }
        json nuovo; //if it doesn't find the key it makes a new node 
        insert(std::make_pair(key, nuovo));//and returns a reference to it 
        return ptr->next->info.second;//how the task required
    }
}
//IN THEORY THEY ARE CORRECT, IF IT STILL DOESN'T WORK I'LL CHECK THEM
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

    private:
        impl::list const* m_ptr;
};



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

    private:
        impl::dict const* m_ptr;
};
/* -----------------------------------------------^^^-iterators-^^^----------------------------------------------- */
/* --------------------------------------------------------------------------------------------------------------- */


//CHECKED
double& json::get_number(){//returns a reference so it can be used to write
    if(is_number()){
        return pimpl->num;
    }else{
        throw json_exception{"at: get_number: obj is not a number"};
    }
}

//CHECKED
double const& json::get_number() const{//read only
    if(is_number()){
        return pimpl->num;
    }else{
        throw json_exception{"at: get_number_const: obj is not a number"};
    }
}

//CHECKED
bool& json::get_bool(){//returns a reference so it can be used to write
    if(is_bool()){
        return pimpl->statement;
    }else{
        throw json_exception{"at: get_bool: obj is not a bool"};
    }
}

//CHECKED
bool const& json::get_bool() const{//read only
    if(is_bool()){
        return pimpl->statement;
    }else{
        throw json_exception{"at: get_bool_const: obj is not a bool"};
    }
}

//CHECKED
std::string& json::get_string(){//returns a reference so it can be used to write
    if(is_string()){
        return pimpl->str;
    }else{
        throw json_exception{"at: get_string: obj is not a string"};
    }
}

//CHECKED
std::string const& json::get_string() const{//read only
    if(is_string()){
        return pimpl->str;
    }else{
        throw json_exception{"at: get_string_const: obj is not a string"};
    }
}

//CHECKED
void json::set_null(){
    if(!is_null()){
        //clears an impl
        pimpl->str = "";
        pimpl->num = inf;
        pimpl->null = true;
        pimpl->is_list = false;
        pimpl->is_dict = false;

        while(pimpl->list_head != nullptr){      //deallocate list
            impl::list* temp = pimpl->list_head->next;
            delete pimpl->list_head;
            pimpl->list_head = temp;
        }
        pimpl->list_tail = nullptr;
        pimpl->list_head = nullptr;

        while(pimpl->dict_head != nullptr){      //deallocate dictionary
            impl::dict* temp = pimpl->dict_head->next;
            delete pimpl->dict_head;
            pimpl->dict_head = temp;
        }
        pimpl->dict_head = nullptr;
    }
    assert(is_null());
}

//CHECKED
void json::set_string(std::string const& s){
    if(is_string()){
        pimpl->str = s;
    }else{
        set_null();
        pimpl->null = false;
        pimpl->str = s;
    }
    assert(is_string());
}

//CHECKED   
void json::set_bool(bool b){
    if(is_bool()){
        pimpl->statement = b;
    }else{
        set_null();
        pimpl->null = false;
        pimpl->statement = b;
    }
    assert(is_bool());
}

//CHECKED
void json::set_number(double x){
    if(is_number()){
        pimpl->num = x;
    }else{
        set_null();
        pimpl->null = false;
        pimpl->num = x;
    }
    assert(is_number());
}

//CHECKED
void json::set_list(){
    set_null();
    pimpl->is_list = true;
    pimpl->null = false;
    assert(is_list());
}

//CHECKED
void json::set_dictionary(){
    set_null();
    pimpl->is_dict = true;
    pimpl->null = false;
    assert(is_dictionary());
}

//USED PROF'S SLIDES
void json::push_front(json const& x) {
    if(is_list()){
        if(pimpl->list_head == nullptr) {
            pimpl->list_head = new impl::list{x, nullptr};
            pimpl->list_tail = pimpl->list_head;
            return;
        }
        pimpl->list_head = new impl::list{x, pimpl->list_head};
    }else   throw json_exception{"at: push_front: obj is not a list"};
}
    
//USED PROF'S SLIDES
void json::push_back(json const& x) {
    if(is_list()){
        if(pimpl->list_head == nullptr){
            push_front(x);
            return;
        }
        pimpl->list_tail->next = new impl::list{x, nullptr};
        pimpl->list_tail = pimpl->list_tail->next;
    }else throw json_exception{"at: push_back: obj is not a list"};
}

//USED PROF'S SLIDES
void json::insert(std::pair<std::string, json> const& x){
    if(is_dictionary()){
        if(pimpl->dict_head == nullptr){
            pimpl->dict_head = new impl::dict{x, nullptr};
        }
        pimpl->dict_head = new impl::dict{x, pimpl->dict_head};
    }else throw json_exception{"at: insert: obj is not a dict"};
}





void LIST_PARSER(std::istream& lhs, json& rhs){    //reads a LIST from lhs and puts it in rhs
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
                        LIST_PARSER(lhs, element);    //reads the next list from input and puts it into element
                        rhs.push_back(element);
                        std::cout << "is a list" << std::endl;
                    }else{
                        if(c == '{'){   //parse a dictionary
                            json element;
                            element.set_dictionary();
                                assert(element.is_dictionary());
                            DICT_PARSER(lhs, element);
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
            LIST_PARSER(lhs, rhs);
        }
    }               //-------------------------------------------------------
    std::cout << "done" << std::endl;
}

//LIST input is done, i have to finish DICT input and then see how to use the possible iterators to output
//everything

void DICT_PARSER(std::istream& lhs, json& rhs){    //reads a DICT from lhs and puts it in rhs
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
                        LIST_PARSER(lhs, element); //populates element with the list in lhs
                        std::pair<std::string, json> info{s, element};
                        rhs.insert(info);
                        std::cout << "is a LIST" << std::endl;
                    }else{
                        if(c == '{'){
                            //parse dictionary
                            json element;
                            element.set_dictionary();
                            DICT_PARSER(lhs, element); //populates element with the dictionary in lhs
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
            DICT_PARSER(lhs, rhs);
        }
    }  
}


    void LIST_PARSER(std::istream& lhs, json& rhs){}
    void DICT_PARSER(std::istream& lhs, json& rhs){}
    void BOOLEAN_PARSER(std::istream& lhs, json& rhs){}
    void NUMBER_PARSER(std::istream& lhs, json& rhs){}
    void NULL_PARSER(std::istream& lhs, json& rhs){}
    void STRING_PARSER(std::istream& lhs, json& rhs){}



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
        LIST_PARSER(lhs, rhs); //must read a list from std::input and put it in rhs
    }else{
        if(c == '{'){   //is a dictionary
            rhs.set_dictionary();
            std::cout << "is a DICT" << std::endl;
            DICT_PARSER(lhs, rhs); //must read a dictionary from std::input and put it in rhs
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

/* you can substitute every writing of "json::impl::..."
with just impl::... */