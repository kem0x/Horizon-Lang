## Operator precedence

| Operators      						| Category or name  |
| -------------  						| ----------------- |
| `x = y`, `x = {...}`      			| Assignment        |
| `x || y`		 						| Conditional OR    |
| `x && y`		 						| Conditional AND   |
| `x == y`, `x != y`	 				| Equality          |
| `x < y`, `x > y`, `x <= y`, `x >= y`  | Comparison        |
| `x + y`, `x - y`						| Additive          |
| `x * y`, `x / y`, `x % y`				| Multiplicative    |
| `x.y`, `f(x)`, `a[i]`, `x++`, `x--`	| Primary     |


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

- Loop statement:

```js
// loops 3 times
loop(3) {
	print "ok";
}

// loops infinitely
let i = 0;
loop {
	if (i == 2)
	{
		i = i + 1;
		continue;
	}

	print i;

	if (i == 5)
	{
		break;
	}

	i = i + 1;
}
```

## Expressions:

Todo:

- [ ] Unary: `-x`, `!x`
- [ ] Incremenet/Decrement operators `x++`, `x--`
- [ ] for loops

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
let bar = { x: 1, y: 1 };

print(bar["x"] == bar.y);
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
    print(x);
}

print(x); // Error: x is not defined
```

- Logic:

```js
// Conditional AND (&& or "and")
"ok" and "ko"  // "false"
"ok" and "ok"  // "true"

// Conditional OR (|| or "or")
"ok" or "ko"  // "ok"
false or "ko" // "ko"

// Equality
"ok" == "ok" // "true"
"ok" == "ko" // "false"

// Inequality
"ok" != "ok" // "false"
"ok" != "ko" // "true"
```

- If:

```js
let x = if (true) { 1 } else { 2 };

if (x == 1) {
    print("Hello World");
    print(if (x != 1) { "hi" } else { "bye" });
}
```

- Functions:

```js
function foo()
{
	return 100;
}

let fmt = format("{}: {}", foo(), 300);

print(fmt) // 100.000000: 300.000000

print("{}", 100) // 100.000000

print(foo(), 300) // 100.000000 300.000000
```