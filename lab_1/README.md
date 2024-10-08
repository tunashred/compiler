## Exercise 1
Create a Regex which could match date formats.

Matching examples:
```
10/10/2023
01.12.23
09/10.2025
1.01.2002
```
Non-matching examples:
```
13/123/2024
01/13/24
0.1.2024
```


### Solution
```
(0?[1-9]|1[012])(\/|\.)(0?[1-9]|1[012])(\/|\.)([12]?[0-9]{2,3})
```

## Exercise 2
Given the regex below, find a matching string:
```
[A-Z][a-zA-Z]*,[a-z]+ [0-9]+(\/.)*(\.|\?)
```

### Solution
```
GNUlinux,something 967331/#.
```

## Exercise 3
Create a Regex which validates a simplified syntax of character classes.

Matching examples:
```
[abc]
[^0-9]
[\r\n\t]
[., !?;\-]
[\\]
```

Non-matching examples:
```
[\]
[\\\]
[a
```

### Solution (until I think of a better one)
```
\^?\[\^?(\\[rnt])*|(\\\\)+|([a-zA-Z0-9_]\-?[a-zA-Z0-9_])*\]\$?
```