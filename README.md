# C++ validator

Modern C++ header-only library for validation of variables and objects. The library can be used with various platforms and compilers that support C++14 and/or C++17 standards. 

The library was tested with clang, gcc and msvc compilers. Tested operating systems include Linux, Windows, macOS, iOS and Android platforms.

_The README and reference documentation is under construction yet._

Currently only doxygen-style comments in sources are available. Tests in test folder can be used as a kind of examples. In sample-build you can find scripts and some description for tests building.

## Some basic examples of usage

### Check if value is greater than constant

```cpp

auto v=validator(gt,100); // define validator

int value1=90;
if (!v.apply(value1))
{
// validation failed
}

int value2=200;
if (v.apply(value2))
{
// validation succeeded
}

```

### Check if string is greater than or equal to other string and size of the string is less than constant

```cpp

auto v=validator(
  value(gte,"sample string"),
  size(lt,15)
); // define validator

std::string str1="sample";
if (!v.apply(str1))
{
// validation failed, string is less than sample string
}

std::string str2="sample string+";
if (v.apply(str2))
{
// validation succeeded
}

std::string str3="too long sample string";
if (!v.apply(str3))
{
// validation failed, string is too long
}

```
### Check if value is in interval and print report

```cpp

auto v=validator(in,interval(95,100));

std::string report;
size_t val=90;
auto ra=make_reporting_adapter(val,report);
if (!v.apply(ra))
{
    std::cerr << report << std::endl; 
    /* will print:
    
    "must be in interval [95,100]"
    
    */
}

```
### Check object member and print report

```cpp

auto v=validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^OR^ value(gte,"zzzzzzzzzzzz"))
            );

std::string report;
std::map<std::string,std::string> test_map={{"field1","value1"}};
auto ra=make_reporting_adapter(test_map,report);

if (!v.apply(ra))
{
    std::cerr << report << std::endl;
    /* will print:
    
    "field1 must be greater than or equal to xxxxxx OR size of field1 must be greater than or equal to 100 OR field1 must be greater than or equal to zzzzzzzzzzzz"
    
    */    
}
    
```