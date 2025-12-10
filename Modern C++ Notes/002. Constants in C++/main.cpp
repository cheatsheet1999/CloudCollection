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