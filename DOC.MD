## Statements:

- Variable declaration:

```js
let x = 1;
const x = 1;
```

- Print statement:

```js
print x;
print "Hello World";
```

## Expressions:

Todo:

- [ ] Unary: `-x`, `!x`

- Literal:

```js
1;
("Hello World");
true;
false;
null;
```

- Variable:

```js
x;
```

- Assignment:

```js
x = 1;
```

- Function call:

```js
f(1, 2, 3);
```

- Object:

```js
{ x: 1, y: 2 };
```

- Member access:

```js
x.y;
x["y"];
```

- Arithmetic:

```js
1 + 2;
1 - 2;
1 * 2;
1 / 2;
1 % 2;
```

- Block/Scope:

```js
{
    let x = 1;
    print x;
}

print x; // Error: x is not defined
```

- Logic:

```js
// Logical AND
true && false;
true and false;

// Logical OR
true || false;
true or false;

// Equality
true == false;
true same false;

// Inequality
true != false;
true not false;
```

- If:

```js
let x = if (true) { 1 } else { 2 };

if (x == 1) {
    print "Hello World";
    print if (x != 1) { "hi" } else { "bye" };
}
```
