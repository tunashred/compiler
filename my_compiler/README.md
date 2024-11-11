## My own programming language
Sounds pretty cool, right? The syntax may suffer changes overtime, but this is not important.

The important thing is that I'm doing it.

## How would it look like?
I will just provide a snippet with _all_ the stuff that it would contain.

```
#const PI 3.14159265358979323

#macro max(a, b) [a < b ? a : b]

#macro printArray(arr, size) [
    int i = 0;
    while i < size {
        print(arr[i]);
        println();
        i++;
    }
]

fun void moo(int a, int b) {
    int x = 3, y, z = 1;
    int i = 0;

    int array_a[10] {0, 1, 2}; // way to initialize?
    int array_b[68] = [0 - 68] {69}; // initialize the 0-68 range of elements to value 69

    if (a < b && b^2 == 64 || x * a == b) {
        while i < 10 {
            array_a[i] = a;
            i++;
        }
        return; // we can do this in void functions
    }

    while (i = 0) < 68 { // here i takes value 0 before checking 'while' condition
        array_b[i] = array_a[i % 10] + 13;
        i++;
    }
}

void main() {
    int x = 12;
    moo -> 0, 12; // not sure if this is a nice way to call a function
    
    switch (x) {
        case 12:
            print("idk what to do here");
            break;
        
        default:
            break;
    }

}
```

### Notes:

#### 1\. Should the macro be on one line only? That means when formatting them, escape characters (`\`) will be needed to avoid newlines.

This programming language should feel a bit more.. modern. I need to think for a solution...
At the moment, macros would need to be wrapped around `[ ]`.

#### 2\. What if I want to somehow debug macros? If so, the way of writting them should be strict and they may contain more info (for example, arg types).

#### 3\. I think for the moment, the only available type of loop is `while`. (if I got enough time, I would try to implement `foreach` loop)

#### 4\. Would be nice to be able to use increment operator with array indexing (`array[i++]`).
