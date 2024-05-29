``` c
struct Node {
	struct BinaryTree *l;
	int x;
	struct BinaryTree *r;
};

Sumtype(
	BinaryTree,
	(int, leaf),
	(struct Node, node)
)

int sum(struct BinaryTree *tree) {
	match(struct BinaryTree, tree) {
		let(int, leaf) return *leaf;
		let(struct Node, node) return sum(node->l) + node->x + sum(node->r);
	}
	// Unreachable
	return 0;
}

//if you are using C2x or GNU extensions, the types can be inferred!
#if Sumtype_typeinference
int sum_inferred(struct BinaryTree *tree) {
	match_t(tree) {
		let_t(leaf) return *leaf;
		let_t(node) return sum(node->l) + node->x + sum(node->r);
	}
	// Unreachable
	return 0;
}
#endif
```

Safe, intuitive [sum types](https://en.wikipedia.org/wiki/Sum_type) with exhaustive pattern matching in a single 100 line header, pure C99.

Inspired by [datatype99](https://github.com/Hirrolot/datatype99), but consisting of
one small standard-conforming C99 macro-only header that is fast to compile.

## Installation
Just include the header file `sumtype.h`.


## Usage
The provided macros are a syntactic sugar over tagged unios. The above code is equivalent to:
```c
struct BinaryTree {
    enum { Leaf, Node } tag;
    union {
        int leaf;
        struct Node node;
    } data;
};

int sum(struct BinaryTree *tree) {
    switch (tree->tag) {
    case Leaf:
        return tree->data.leaf;
    case Node:
        return sum(tree->data.node.lhs) + tree->data.node.x + sum(tree->data.node.rhs);
    }

    // Unreachable.
    return 0;
}
```

The data type is generated with the `Sumtype` macro, which takes the name of the resulting
struct and and list of pairs of types and names of the variants, all separated by commas.
```c
Sumtype(Haha, (char *, name), (uint16_t, id), (uint32_t, var))
```

You can pattern-match over the instances of this type. The `match` macro takes a type
and an expression evaluating to a pointer to a value of this type.
Then, for each variant, the `let` macro takes a type and name of the given variant.
If the value being matched is of this variant, the `let` macro generates a binding
to a pointer to a value of this type with the provided name, which you may use in the
following statement.
```c
match(struct Haha, &something) {
	let(char *, name) {
    		printf("%s\n", *name);
	}
	let(uint16_t, id) {
    		printf("%d\n", *id);
	}
	otherwise {
    		printf("var\n");
	}
};
```
`otherwise` matches all variants.

For each variant, a constructor with the name `Typename_variantname` is generated.

```c
struct BinaryTree leaf5 = BinaryTree_leaf(5);
```

If you want to just test for one variant, the `iflet` macro is provided, which takes
the sum type, the type and the name of the variant being matched and an expression that evaluates
to a pointer of the sum type. It provides a binding analogical to `let` in `match`.
```c
iflet(struct BinaryTree, int, leaf, &tree) {
	printf("A leaf with the value %d\n", *leaf);
} else {
	printf("Not a leaf\n");
}

#if Sumtype_typeinference

iflet_t(leaf, &tree) {
	printf("A leaf with the value %d\n", *leaf);
} else {
	printf("Not a leaf\n");
}
#endif
```

Finally, the `MATCHES` macro, taking a name of a variant and an expression, tests whether
the value of the expression is of the given variant.

## Comparison to datatype99
datatype99 is written using an impressive functional language built atop of preprocessor
macros, [Metalang99](https://github.com/Hirrolot/metalang99). While this is a powerful
language and the resulting macros are more flexible, there are several reasons why
`sumtype.h` may be considered:
- It is a 100 line single header that you can just copy into your project and even quickly
  understand and modify to your liking.
- The compile times are much faster.
- It allows matching against `const` pointers.
- It doesn't pollute the namespace with any typedefs. It only expands to a struct with
  the provided name (contrary to a separate new type for all variants).

## Syntax and semantics

### EBNF syntax

```ebnf
<sumtype>       ::= "Sumtype(" <ident> { "," "(" <type>, <ident> ")" }+ ")" ;

<match>         ::= "match(" <type> "," <expr> ") {" { <let> }* [ <otherwise> ] "}" ;
<of>            ::= "let(" <type> "," <variant-name> ")" <stmt> ;
<otherwise>     ::= "otherwise" <stmt> ;

<if-let>        ::= "iflet(" <type> "," <type> "," <ident> "," <expr> ")" <stmt> ;
<matches>       ::= "MATCHES(" <ident> "," <expr> ")" ;
```

### Semantics

The code
```c
Sumtype(A, (Type0, name0), ..., (TypeN, nameN))
```
is expanded to
```c
struct A {
	enum {
		SUMTYPE_TAG_name0,
		...
		SUMTYPE_TAG_nameN,
	} tag;
	union {
		Type0 name0;
		...
		TypeN nameN;
	} variant;
}

struct A A_name0(Type0 name0) {
	return (struct A) {
		.tag = SUMTYPE_TAG_name0,
		.variant = {
			.name0 = name0
		}
	}
}

...

struct A A_nameN(TypeN nameN) {
	return (struct A) {
		.tag = SUMTYPE_TAG_nameN,
		.variant = {
			.nameN = nameN
		}
	}
}
```

#### `match`

`match` has the expected semantics: it sequentially tries to match the given instance of
a sum type against the given variants, and, if a match has succeeded, it executes the
corresponding statement and moves down to the next instruction
(`match(val) { ... } next-instruction;`). If all the matches have failed,
it executes the statement after `otherwise` and moves down to the next instruction.

A complete `match` construct results in a single C statement.

#### `let`

`let` accepts a type and a matched variant name as its arguments. If the variant is matched,
it binds a variable with the provided name to a pointer to the value of the provided type.

#### `iflet`

`iflet` tries to match the given instance of a sum type against the given variant,
and, if a match has succeeded, it executes the corresponding statement.

A complete `iflet` construct results in a single C statement.

#### `MATCHES`

`MATCHES` just tests an instance of a sum type for a given variant.
If the given instance corresponds to the given variant, it expands to truthfulness, otherwise it expands to falsehood.


## Caveats

### Unique variant name

The name of all the variants used, even in different types, need to be unique.
Otherwise, the names fo their tags will collide.

### Top-level `break`/`continue`

`break`/`continue` inside a statement provided to `let`/`iflet`
but outside of any `for`/`while` loops in that statement just terminates the `match`/`iflet`
statement, but not any outside loop. Use a `goto` to break out of the outside loop.

### Mutable bindings

Bindings introduced by `let` are mutable, unless you write `const` in front of the name
of the type. The compiler will warn you about this if you match against a const pointer.

## License

Public domain or WTFPL, at your choice.
