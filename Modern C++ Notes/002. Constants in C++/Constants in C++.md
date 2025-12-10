# **📘 Constants in C++**

Constants are values in a program that **cannot be modified once defined**.  
C++ provides two primary ways to declare constants: **macro constants** and **const variables**.

---

## **1. What Are Constants?**

### **① A constant stores data that must remain unchanged during program execution.**

### **② Two common ways to define constants in C++**

---

### **🔹 Method 1: Macro Constant (`#define`)**

```cpp
#define CONSTANT_NAME value
```

> **Note**  
> Macro constants are usually placed at the *top* of the file and are replaced by the preprocessor before compilation.

---

### **🔹 Method 2: Constant Variable (`const`)**

```cpp
const data_type CONSTANT_NAME = value;
```

> **Note**  
> Adding `const` before a variable makes it read-only. Attempts to modify it will cause a compilation error.

---

# **2. Example: Using `#define`**

```cpp
#include <iostream>
using namespace std;

#define Day 7 

int main()
{
    /*
    Day = 14;  // ❌ Error: Day is a constant and cannot be modified
    */

    cout << "A week has: " << Day << " days" << endl;

    system("pause");
    return 0;
}
```

### **💻 Program Output**

```
A week has: 7 days
Press any key to continue . . .
```

---

# **3. Example: Using `const`**

```cpp
#include <iostream>
using namespace std;

int main()
{
    /*
    const int month = 12;
    month = 24;   // ❌ Error: const variables cannot be reassigned
    */

    int month = 12;
    month = 24;   // ✔️ Allowed: regular variables can be modified

    cout << "A year has: " << month << " months" << endl;

    system("pause");
    return 0;
}
```

### **💻 Program Output**

```
A year has: 24 months
Press any key to continue . . .
```

---

# **✨ Summary**

| Method | Syntax | Can Modify? | Typical Use |
|--------|--------|--------------|--------------|
| **`#define` Macro** | `#define Name value` | ❌ No | Global constants, compile-time replacement |
| **`const` Variable** | `const int x = value;` | ❌ No | Type-safe constants, scoped constants |

---
