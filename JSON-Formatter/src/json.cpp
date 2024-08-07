#include "../include/json.hpp"
#include "iterators.cpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
static constexpr double inf = std::numeric_limits<double>::max();

int layer = 0;

/*
 * TO DO:
 * modificare tutta la semantica come spiegata da Spanò in PO
 */



#define DEFAULT 	"\033[0m"
#define BLACK		"\033[0;30m"
#define RED 		"\033[0;31m"		
#define GREEN		"\033[0;32m"
#define YELLOW		"\033[0;33m"
#define BLUE		"\033[0;34m"
#define PURPLE		"\033[0;35m"
#define CYAN		"\033[0;36m"
#define WHITE		"\033[0;37m"

std::vector<std::string> colors={RED, GREEN, YELLOW, BLUE, PURPLE, CYAN};

struct json::impl {
    std::string str;
    bool is_string;
    double num;
    bool statement;
    bool null;
//if null is true then statement doesn't matter

    bool is_list;           //these are required because the list or dict could exist but be empty
    bool is_dict;
    struct list {
        json info;
        list *next;
    };

    list *list_head;
    list *list_tail;

    struct dict
    {
        std::pair<std::string,json> info;
        dict* next;
    };

    dict* dict_head;
    dict* dict_tail;


    impl()
    {
        str = "";
        is_string = false;
        num = inf;
        statement = false;   //value is not relevant since I'll be checking null
        null = true;         //if this is true then statement is not relevant
        // except for is_list and is_dict that must be false to be coherent
        is_list = false;
        is_dict = false;
        list_head = nullptr;
        dict_head = nullptr;
        list_tail = nullptr;
        dict_tail = nullptr;
    }

};

    std::istream& LIST_PARSER(std::istream& lhs, json& rhs);
    std::istream& DICT_PARSER(std::istream& lhs, json& rhs);
    std::istream& BOOLEAN_PARSER(std::istream& lhs, json& rhs);
    std::istream& NUMBER_PARSER(std::istream& lhs, json& rhs);
    std::istream& NULL_PARSER(std::istream& lhs, json& rhs);
    std::istream& STRING_PARSER(std::istream& lhs, json& rhs);

    void LIST_PRINT(std::ostream& lhs, json const& rhs);//is required const for the output and input signatures
    void DICT_PRINT(std::ostream& lhs, json const& rhs);

    void MINIFIER(std::ostream& lhs, json const& rhs);

    std::string to_lower_case(std::string str);


//CHECKED
json::json(){       //allocates a null json-type object
    pimpl = new impl(); //implemented
}

//CHECKED
json::json(const json& j){          //copy constructor
    pimpl = new impl;

    pimpl->str = j.pimpl->str;
    pimpl->is_string = j.pimpl->is_string;
    pimpl->num = j.pimpl->num;
    pimpl->statement = j.pimpl->statement;
    pimpl->null = j.pimpl->null;

    pimpl->is_list = false;
    pimpl->is_dict = false;
    pimpl->list_head = nullptr;
    pimpl->dict_head = nullptr;
    pimpl->list_tail = nullptr;
    pimpl->dict_tail = nullptr;

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
json::json(json&& j) noexcept{       //move constructor
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
json& json::operator=(const json& j){
    if(this != &j){        //if this and j are not the same obj
        if(!is_null()) set_null();        //clear this

        if(j.is_list()){
            set_list();
            impl::list* temp = j.pimpl->list_head;
            while(temp != nullptr){
                push_back(temp->info);
                temp = temp->next;
            }
        }else{
            if(j.is_dictionary()){
                set_dictionary();
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
        pimpl->is_string = j.pimpl->is_string;
        pimpl->is_list = j.pimpl->is_list;
        pimpl->is_dict = j.pimpl->is_dict;
    }
    return *this;
}

//CHECKED   
json& json::operator=(json&& j) noexcept{            //move operator
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
            !(pimpl->is_string);
    return check;
}
//CHECKED
bool json::is_dictionary() const{
    bool check;
    check = !(pimpl->is_list)   &&
            pimpl->is_dict      &&
            !(pimpl->null)      &&
            pimpl->num == inf   &&
            !(pimpl->is_string);
    return check;
}
//CHECKED
bool json::is_string() const{
    bool check;
    check = !(pimpl->is_list)   &&
            !(pimpl->is_dict)   &&
            !(pimpl->null)      &&
            pimpl->num == inf   &&
            pimpl->is_string;
    return check;
}
//CHECKED
bool json::is_number() const{
    bool check;
    check = !(pimpl->is_list)   &&
            !(pimpl->is_dict)   &&
            !(pimpl->null)      &&
            pimpl->num != inf   &&
            !(pimpl->is_string);
    return check;
}
//CHECKED
bool json::is_bool() const{
    bool check;
    check = !(pimpl->is_list)   &&
            !(pimpl->is_dict)   &&
            !(pimpl->null)      &&
            pimpl->num == inf   &&
            !(pimpl->is_string);
    return check;
}
//CHECKED
bool json::is_null() const{
    bool check;
    check = !(pimpl->is_list)   &&
            !(pimpl->is_dict)   &&
            pimpl->null         &&
            pimpl->num == inf   &&
            !(pimpl->is_string);
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
        throw json_exception{"at: op[]: obj is not a dict"};
    }else{
        impl::dict* ptr = pimpl->dict_head;
        while(ptr != nullptr){
            if(ptr->info.first == key){
                return ptr->info.second;//if it finds the key it returns the value
            }
            ptr = ptr->next;
        }
        json nuovo; //if it doesn't find the key it makes a new node 
        insert(std::pair<std::string, json> {key, nuovo});//and returns a reference to it
        return pimpl->dict_tail->info.second;//how the task required
    }
}


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
        pimpl->is_string = false;
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
        pimpl->dict_tail = nullptr;
    }
}

//CHECKED
void json::set_string(std::string const& s){
    if(is_string()){
        pimpl->str = s;
    }else{
        if(!is_null()) set_null();
        pimpl->is_string = true;
        pimpl->null = false;
        pimpl->str = s;
    }
}

//CHECKED   
void json::set_bool(bool b){
    if(is_bool()){
        pimpl->statement = b;
    }else{
        if(!is_null()) set_null();
        pimpl->null = false;
        pimpl->statement = b;
    }
}

//CHECKED
void json::set_number(double x){
    if(is_number()){
        pimpl->num = x;
    }else{
        if(!is_null()) set_null();
        pimpl->null = false;
        pimpl->num = x;
    }
}

//CHECKED
void json::set_list(){
    if(!is_null()) set_null();
    pimpl->is_list = true;
    pimpl->null = false;
}

//CHECKED
void json::set_dictionary(){
    if(!is_null()) set_null();
    pimpl->is_dict = true;
    pimpl->null = false;
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
    }else throw json_exception{"at: push_front: obj is not a list"};
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
            pimpl->dict_tail = pimpl->dict_head;
        }else{
            pimpl->dict_tail->next = new impl::dict{x, nullptr}; 
            pimpl->dict_tail = pimpl->dict_tail->next;
        } 
    }else throw json_exception{"at: insert: obj is not a dict"};
}


//can't use switch because the statements are not literal
std::istream& LIST_PARSER(std::istream& lhs, json& rhs){
    char c;
    lhs >> c;       //reads the first char after the opening [ to see what it needs to parse 
    if(c == ']'){       //empty list
        lhs.putback(c);
        return lhs;    
    }
    if(c == '"'){   //parses a string, without considering spaces and tabs
        lhs.putback(c);
        json element;
        STRING_PARSER(lhs, element);       //should read the string from lhs and put it into element
        rhs.push_back(element);
    }else{

        if((c >= 48 && c <= 57) || c == '+' || c == '-'){ //parse a double
            lhs.putback(c);
            json element;
            NUMBER_PARSER(lhs, element);
            rhs.push_back(element);
        }else{

            if(c == 't' ||  c == 'f'){  //parse a boolean
                lhs.putback(c);
                json element;
                BOOLEAN_PARSER(lhs, element);
                rhs.push_back(element);
            }else{

                if(c == 'n'){   //parse a NULL
                    lhs.putback(c);
                    json element;
                    NULL_PARSER(lhs, element);
                    rhs.push_back(element);
                }else{

                    if(c == '['){   //parse a list
                        json element;
                        element.set_list();
                        layer++;
                        LIST_PARSER(lhs, element);    //reads the next list from input and puts it into element
                        layer--;
                        rhs.push_back(element);
                        lhs >> c;
                    }else{

                        if(c == '{'){   //parse a dictionary
                            json element;
                            element.set_dictionary();
                            layer++;
                            DICT_PARSER(lhs, element);
                            layer--;
                            rhs.push_back(element);
                            lhs >> c;
                        }
                    }
                }
            }
        }
    }

        lhs >> c;
        if(c == ']')
            lhs.putback(c);
        if(c == ',')
            LIST_PARSER(lhs, rhs);
        
        return lhs;
    }

std::istream& DICT_PARSER(std::istream& lhs, json& rhs){
    char c;
    std::string key;
    lhs >> c;   //reads  the "
    if(c == '}'){
        lhs.putback(c);
        return lhs;
    } 
    lhs >> c;   //skips the "
    do{
        while(c == 92){
            key += c;
            lhs.get(c);       //if there's an escape consume another char
        }
        key += c;
        lhs.get(c); 
    }while(c != '"');
    lhs >> c;
    
    if(c != ':'){
        throw json_exception{"at: DICT input: char is not \":\""};
    }
                    //now s is the key, I have to parse the correspondent JSON

    lhs >> c;
    if(c == '"'){   //parses a string, without considering spaces and tabs
        lhs.putback(c);
        json element;
        STRING_PARSER(lhs, element);
        std::pair<std::string, json> info{key, element};
        rhs.insert(info);

    }else{
        if((c >= 48 && c <= 57) || c == '+' || c == '-'){
            lhs.putback(c);
            json element;
            NUMBER_PARSER(lhs, element);
            std::pair<std::string, json> info{key, element};
            rhs.insert(info);

        }else{
            if(c == 't' || c == 'f'){
            lhs.putback(c);
            json element;
            BOOLEAN_PARSER(lhs, element);
            std::pair<std::string, json> info{key, element};
            rhs.insert(info);

            }else{
                if(c == 'n'){   //parse a NULL
                    lhs.putback(c);
                    json element;
                    NULL_PARSER(lhs, element);
                    std::pair<std::string, json> info{key, element};
                    rhs.insert(info);

                }else{
                    if(c == '['){
                        //parse list
                        json element;
                        element.set_list();
                        layer++;
                        LIST_PARSER(lhs, element); //populates element with the list in lhs
                        layer--;
                        std::pair<std::string, json> info{key, element};
                        rhs.insert(info);
                        lhs >> c;

                    }else{
                        if(c == '{'){
                            //parse dictionary
                            json element;
                            element.set_dictionary();
                            layer++;
                            DICT_PARSER(lhs, element); //populates element with the dictionary in lhs
                            layer--;
                            std::pair<std::string, json> info{key, element};
                            rhs.insert(info);
                            lhs >> c;   //must be the }
                        }
                    }
                }
            }
        }
    }
    lhs >> c;
    if(c == '}')
            lhs.putback(c);
    if(c == ',')
        DICT_PARSER(lhs, rhs);

    return lhs;
}


    std::istream& BOOLEAN_PARSER(std::istream& lhs, json& rhs){
        std::string s;
        char c;
        char peek = lhs.peek();
        while(peek == 't' || peek == 'r' || peek == 'u' || peek == 'e'
            || peek == 'f' || peek == 'a' || peek == 'l' || peek == 's'){
            lhs >> c;
            s += c;
            peek = lhs.peek();
        }
        if(s == "true"){ 
            rhs.set_bool(true);
        }else{
            if(s == "false"){
                rhs.set_bool(false);
            }else{              //not true nor false
                throw json_exception{"at: BOOLEAN_PARSER input: obj read is neither true or false " + s};
            }
        }
        return lhs;
    }

    std::istream& NUMBER_PARSER(std::istream& lhs, json& rhs){
        double num;
        lhs >> num;
        rhs.set_number(num);        //of course does not read eventual , } ]
        return lhs;
    }

    std::istream& NULL_PARSER(std::istream& lhs, json& rhs){
        std::string s;
        char c;
        char peek = lhs.peek();
        while(peek == 'n' || peek == 'u' || peek == 'l'){
            lhs >> c;
            s += c;
            peek = lhs.peek();
        }
        if(s == "null"){
            rhs.set_null();
        }else{
            throw json_exception{"at: NULL_PARSER input: obj read is not a null " + s};
        }
        return lhs;
    }

    std::istream& STRING_PARSER(std::istream& lhs, json& rhs){
        std::string s;
        char c;
        lhs >> c;   //consumes '"'
        lhs.get(c);
        if(c == '"'){
            rhs.set_string("");
            return lhs;
        }
        do{
            while(c == 92){ //in this way if there's a '\\\\"' no matter how many \ there are it is working
                s += c;
                lhs.get(c);
            }
            s += c;
            lhs.get(c);
        }while(c != '"');
        
        
        rhs.set_string(s);
        return lhs;
    }



void LIST_PRINT(std::ostream& lhs, json const& rhs){
    json::const_list_iterator it = rhs.begin_list();
    while(it != rhs.end_list()){

        for(int i = layer; i > 0; i--){
            lhs << "    ";
        }
        lhs << *it;     //right here *it is a json so it will recursively call the output operator
        it++;
        if(it != rhs.end_list()){
            lhs << ",";
        }
        lhs << std::endl;
    }
}


void DICT_PRINT(std::ostream& lhs, json const& rhs){
    json::const_dictionary_iterator it = rhs.begin_dictionary();
    while(it != rhs.end_dictionary()){
        for(int i = layer; i > 0; i--){
            lhs << "    ";
        }
        lhs << "\"";
        lhs << BLUE;
        lhs << it->first;
        lhs << DEFAULT;
        lhs << "\" : ";
        lhs << it->second;
        it++;
        if(it != rhs.end_dictionary()){
            lhs << ", ";
        }
        lhs << std::endl;
    }
}

//OUTPUT
std::ostream& operator<<(std::ostream& lhs, json const& rhs){   //takes inputs from rhs and puts them into lhs (lhs << rhs)
    if(rhs.is_bool()){
        bool statement = rhs.get_bool();    //if you just do lhs << rhs.get_bool() it prints 1/0
        if(statement)
            lhs << PURPLE << "true" << DEFAULT;
        else lhs << PURPLE << "false" << DEFAULT;
    }else{
        if(rhs.is_null()){
            lhs << "null";
        }else{
            if(rhs.is_number()){
                lhs << RED << rhs.get_number() << DEFAULT;
            }else{
                if(rhs.is_string()){
                    if(to_lower_case(rhs.get_string()) == "simone"){
                        lhs << '\"';
                        for(int i = 0; i < 6; i++){
                            lhs << colors[i];
                            lhs << rhs.get_string()[i];
                        }
                        lhs << DEFAULT;
                        lhs << '\"';
                    }else{
                    lhs << '\"' << GREEN << rhs.get_string() << DEFAULT << '\"';
                    }
                }else{
                    if(rhs.is_list()){
                        lhs << "[";
                        layer++;
                        lhs << std::endl;
                        LIST_PRINT(lhs, rhs);
                        layer--;
                        for(int i = layer; i > 0; i--){
                            lhs << "    ";
                        }
                        lhs << "]";
                    }else{
                        if(rhs.is_dictionary()){
                            lhs << "{";
                            layer++;
                            lhs << std::endl;
                            DICT_PRINT(lhs, rhs);
                            layer--;
                            for(int i = layer; i > 0; i--){
                                lhs << "    ";
                            }
                            lhs << "}";
                        }
                    }
                }
            }
        }
    }
    return lhs;
}

//INPUT
std::istream& operator>>(std::istream& lhs, json& rhs){ //takes inputs from lhs >> and 
                                                        //puts them into rhs (lhs >> rhs)
    char c;
    lhs >> c;
    if(c == '['){   //is a list
        rhs.set_list(); //creates an empty list, i need to add 
                        //the members with push_back, this is handled by LIST()
        LIST_PARSER(lhs, rhs); //must read a list from std::input and put it in rhs
        lhs >> c;   //reads the closing ] from input
    }else{
        if(c == '{'){   //is a dictionary
            rhs.set_dictionary();
            DICT_PARSER(lhs, rhs); //must read a dictionary from std::input and put it in rhs
            lhs >> c;   //reads the closing } from input
        }    
    }
    return lhs;
}

std::string to_lower_case(std::string str){
    std::string res;
    for(auto e : str)  
        res+= (e >= 65 && e <= 90) ? e+32 : e;   //scritta da marius 
    return res;
}



#pragma clang diagnostic pop