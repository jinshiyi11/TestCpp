# C++ 各种问题收集
## C++语法
- new运算符重载,Allocator实现 
## 模板
- 模板文章
  * http://www.codeproject.com/Articles/257589/An-Idiots-Guide-to-Cplusplus-Templates-Part
  * http://www.codeproject.com/Articles/268849/An-Idiots-Guide-to-Cplusplus-Templates-Part
- 实现一个单例模板
- template&lt; classFunction, class... Args &gt; explicitThread(Function&amp;&amp; f, Args&amp;&amp;... args);
- 成员函数模板(类模板中的成员函数也是模板) http://en.cppreference.com/w/cpp/language/member_template
```c++
template<typename T1>
struct string {
    // member template function
    template<typename T2>
    int compare(const T2&);
    // constructors can be templates too
    template<typename T2>
    string(const std::basic_string<T2>& s) { /*...*/ }
};
// out of class definition of string<T1>::compare<T2> 
template<typename T1> // for the enclosing class template
template<typename T2> // for the member template
int string<T1>::compare(const T2& s) { /* ... */ }
```

模板参数类型声明为class... Args怎么用？
- 实现一个unique_ptr,unique_ptr的Deleter 模板参数实现，如何自定义Deleter
- noncopyable模板实现

- c++中可变参数怎么用？
- std::_lock\_guard_的用法
- const
- 运算符重载 仿函数 非运算符重载
- 函数默认实参
- c++ volatile
- 子类的有参构造函数调用基类的有参构造函数
- 构造函数 explicit，基类，数据成员顺序，vptr初始化时间，只允许在栈上创建，只允许在堆上创建
- 如果一个类实现了拷贝构造函数，但是什么都不做会怎么样，调用拷贝构造函数之后数据成员会被初始化吗？(不会)。如果该类有虚函数，调用拷贝构造函数之后它的vptr会被初始化吗？(会)
- 析构函数
- 为什么构造函数不能是虚函数，为什么析构函数不能是纯虚函数，在构造函数中调用虚函数会怎样
- 构造函数try catch，析构函数try catch
- 智能指针
- 虚函数
- static 静态局部变量
- 模板特化，局部特化,单例模板
- std::ref作用，为什么需要？
- Non-type template arguments
- 多重继承
- const\_cast,reinterpret\_cast,dynamic\_cast,...
- inline
- namespace
- c++异常，栈展开，catch all
- 宏# ##
- friend
- std::tuple,std::get,std::tie
- std::function,std::bind bind virtual?
- std::ref,std::cref
- #include&lt;locale&gt;

#include&lt;codecvt&gt;

#include&lt;string&gt;

std::wstring\_convert&lt;std::codecvt\_utf8\_utf16&lt;wchar\_t&gt;&gt; converter;

std::string narrow = converter.to\_bytes(wide\_utf16\_source\_string);

std::wstring wide = converter.from\_bytes(narrow\_utf8\_source\_string);

- 什么是POD类型
- 构造函数中调用memset(this,0,sizeof(\*this))会有什么问题？vptr也被设为null了
- iterator,erase的实现,在遍历过程中erase导致崩溃
- 正向遍历删除和反向遍历删除，erase 函数都只接受正向迭代器 iterator，正向/反向迭代器的关系，如何把反向迭代器转为正向迭代器
