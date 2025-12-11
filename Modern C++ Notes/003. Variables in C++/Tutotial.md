# Variables in C++

## What Is a Variable?

A **variable** is simply a name given to a specific block of memory, allowing us to read from or write to that memory easily.

Key points:

1. A variable gives a name to a memory location so we can operate on it conveniently.
2. Syntax:  
   **`data_type variable_name = initial_value;`**
3. The purpose of variables is to help us manage memory in a clear and structured way.
4. Every piece of memory has its own unique address.
5. C++ requires that every variable or constant must declare its **data type**, otherwise the compiler cannot allocate memory correctly.

---

## Basic Example

```cpp
#include <iostream>
using namespace std;

int main()
{
    // Syntax for creating a variable:
    // data_type variable_name = initial_value;
    int a = 10;

    // Output statement:
    // cout << "Message" << variable << endl;
    cout << "a = " << a << endl;

    system("pause");
    return 0;
}
```

### Program Output

```
a = 10
Press any key to continue . . .
```

---

## Summary

Variables are fundamental building blocks in C++. By naming a memory location and specifying its data type, we can store values, modify them, and use them throughout our program with ease.
