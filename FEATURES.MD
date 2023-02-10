## Operator precedence

| Operators                            | Category or name |
| ------------------------------------ | ---------------- |
| `x = y`, `x = {...}`                 | Assignment       |
| `x \|\| y`                             | Conditional OR   |
| `x && y`                             | Conditional AND  |
| `x == y`, `x != y`                   | Equality         |
| `x < y`, `x > y`, `x <= y`, `x >= y` | Comparison       |
| `x + y`, `x - y`                     | Additive         |
| `x * y`, `x / y`, `x % y`            | Multiplicative   |
| `x.y`, `f(x)`, `a[i]`, `x++`, `x--`  | Primary          |

## Statements:

- Variable declaration:

```haxe
let x: Int = 1;
// const x: Int = 1;
let y: String = "Hello World";
let z: Bool = true;
let a: Array = [1, 2, 3];
let b: Object = { x: 1, y: 1 };
let c: Float = 1.0;
// let d: Enum = Squash;
let e: Any = null;
```

- Class declaration:

```haxe
class IceCream
{
	IceCream(flavor)
	{
		this.flavor = flavor;
	}

	getFlavor()
	{
		return this.flavor;
	}

	serveOn()
	{
		return "Cones";
	}
};

let food: IceCream = IceCream("badFlavor");

food = IceCream("coffee");

print(food.getFlavor());

let servedOn: String = food.serveOn();

print(servedOn);
```

- Enum declaration:

```haxe
enum Veggie
{
  Squash,
  Cabbage,
  Broccoli,
}

let cart: Array = [Cabbage, Cabbage, Broccoli];

let veggie: Enum = Broccoli;

print(veggie);

if (veggie == Broccoli)
{
	print("We've got the best vegetable!");
}
```

- Builtin functions:

```haxe
print("Hello {}", "World");
format("Hello {}", "World");
sleep(1000);
```

- Loop statement:

```haxe
// loops 3 times
loop(3) {
	print("ok");
}

// loops infinitely
let i: Int = 0;
loop {
	if (i == 2)
	{
		i = i + 1;
		continue;
	}

	print(i);

	if (i == 5)
	{
		break;
	}

	i = i + 1;
}
```

- Sync statement:

```haxe
sync
{
	loop (50)
	{
		print("ok");
	}
}

sync
{
	loop (50)
	{
		print("ok");
	}
}
```

## Expressions:

- Literal:

```haxe
1;
("Hello World");
true;
false;
null;
```

- Variable:

```haxe
x;
```

- Array:

```haxe
let x: Array = [1, 2, 3];
```

- Assignment:

```haxe
x = 1;
```

- Function call:

```haxe
f(1, 2, 3);
```

- Object:

```haxe
let bar: Object = { x: 1, y: 1 };

print(bar["x"] == bar.y);
```

- Arithmetic:

```haxe
1 + 2;
1 - 2;
1 * 2;
1 / 2;
1 % 2;
```

- Block/Scope:

```haxe
{
    let x: Int = 1;
    print(x);
}

print(x); // Error: x is not defined
```

- Logic:

```haxe
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

```haxe
let x: Int = if (true) { 1 } else { 2 };

if (x == 1) {
    print("Hello World");
    print(if (x != 1) { "hi" } else { "bye" });
}
```

- Functions:

```haxe
function foo(): Int
{
	return 100;
}

let fmt: String = format("{}: {}", foo(), 300);

print(fmt)

print("{}", 100)

print(foo(), 300)
```
