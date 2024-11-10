## My own programming language
Sounds pretty cool, right? The syntax may suffer changes overtime, but this is not important.

The important thing is that I'm doing it.

## How would it look like?
I will just provide a snippet with _all_ the stuff that it would contain.

```
#const PI 3.14159265358979323

#macro max(a, b) [a < b ? a : b]

#macro printArray(arr, size) [
    for (int i = 0; i < size; i++) {
        print(arr[i]);
        println();
    }
]

fun moo(int a, int b) {
    int x = 3, y, z = 1; // allows multiple declarations and initialization
    
}
```

Notes:

#### 1\. Should the macro be on one line only? That means when formatting them, escape characters (`\`) will be needed to avoid newlines.

This programming language should feel a bit more.. modern. I need to think for a solution.

#### 2\. What if I want to somehow debug macros? If so, the way of writting them should be strict and they may contain more info (for example, arg types)