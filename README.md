# langnd

> langnd is one programmer's dream programming language that focuses on
> minimalism, consistency, and portability.

This project is the home of the langnd programming language. Both the design
and the implementation of the language are a work in progress.

    #!/usr/local/bin/langnd

    $warn=
    <
      # writes a message with a highlighted warning

      $message = argument

      import "write" from core

      $write("\a027[7;33m WARN \a027[0m ", 1)
      $write($message, 1)
      $write("\n", 1)
    >

    $warn("writing langnd code is addictive!")

## Language

**Comments**

    # Line comment

**Nulls**

    null

**Booleans**

    false
    true

**Numbers**

    0
    100
    -200
    +300
    400.500
    -600.700
    +800.900

**Strings**

    ""
    "Plain Text"
    "\"Special\" Text"
    "\a066\a105\a110\a097\a114\a121 Text"

**Lists**

    []
    [null, false, 0, "", [], {}, <>]

**Maps**

    {}
    {"null": null, "boolean": false, "number": 0, "list": [], "map": {}, "function": <>}

**Functions**

    <>
    <"first" "..." "result">
    <return "result">
    <
      # double the input

      $number = argument

      import "multiply" from core

      return $multiply($number, $number)
    >

**Identifiers**

    $variable
    $variable_with_underscores
    $VARIABLE_WITH_UPPERCASES
    $variable_with_02_numbers
    $"variable with spaces"
    $"variable with \"specials\""
    $"variable with \a098\a105\a110\a097\a114\a121"

**Assignment**

    $variable = "value"
    $reference = $variable
    $result = $call()
    $decision = if $pick() "this" otherwise "that"
    $last = while $more() $take()
    $error = catch $failable()

**Branching**

    if $first
    \

    /
    else if $next
    \

    /
    else if $call()
    \

    /
    otherwise
    \

    /

**Looping**

    while $condition
    \

    /

    while $test()
    \

    /

    while true
    \
      break $pick
    /

    while true
    \
      continue $pick
    /

**Errors**

    throw "Error: Invalid format!"
    throw 137
    throw true
    throw ["issue 12", "issue 48"]
    throw {"message": "not found", "code": 404}
    throw $error
    throw $call()

    $error = catch
    \

    /

**Importing**

    import "add" from core
    import ["add", "subtract", "multiply", "divide"] from core
    import {"add": "+", "subtract", "-", "multiply": "*", "divide": "/"} from core
    import $functions from core

**Core**

    $add(75, 25)                                        # 100
    $subtract(500, 400)                                 # 100
    $multiply(20, 5)                                    # 100
    $divide(400, 4)                                     # 100
    $modulo(1000, 900)                                  # 100
    $truncate(100.200)                                  # 100
    $and(true, true)                                    # true
    $or(false, true)                                    # true
    $not(false)                                         # true
    $precedes(0, 100)                                   # true
    $succeeds(100, 0)                                   # true
    $equals(100, 100)                                   # true
    $write("hello file!", "/tmp/hello.txt")             # null
    $read("/tmp/hello.txt", null)                       # "hello file!"
    $delete("/tmp/hello.txt")                           # null
    $query("USER")                                      # "wareification"
    $evaluate("import \"add\" from core $add(99, 1)")   # 100
    $freeze([1, 2, 3])                                  # "[1, 2, 3]"
    $thaw("[3, 2, 1]")                                  # [3, 2, 1]
    $type("mystery")                                    # "STRING"
    $cast("100", "NUMBER")                              # 100
    $get(["a", "b", "c"], 2)                            # "b"
    $set(["a", "b", "?"], 3, "c")                       # ["a", "b", "c"]
    $unset(["a", "b", "uh...", "c"], 3)                 # ["a", "b", "c"]
    $merge(["a"], ["b", "c"])                           # ["a", "b", "c"]
    $length(["a", "b", "c"])                            # 3
    $keys({"a": 1, "b": 2, "c": 3})                     # ["a", "b", "c"]
    $sort([3, 2, 5, 1, 4], "+")                         # [1, 2, 3, 4, 5]

    $inputs                                             # ["-m", "fix: ui"]

## License

langnd is licensed under the Unlicensed license. Review the UNLICENSE file
included in the project before using langnd to understand your rights.

## Building

The reference implementation is written using the 1989 ANSI standard of C. An
ANSI-compliant compiler is required to build the program from source. Consider
the following minimums of the target data model additional requirements:

- `int` must be able to hold all values between `[-2147483647, 2147483647]`
- `long` must be able to hold all values between
`[-9223372036854775807, 9223372036854775807]`
- `size_t` must be able to hold all values between `[0, 2147483647]`

Builds are optionally automated using Make. Only POSIX features are used in
the Makefile. Try one of the available targets: `all`, `clean`, `check`,
`install`, `uninstall`.

Perform a fresh build of the program, check build passes tests, then install it
to the system:

    make clean check install

## Running

Once you've built and installed the program you'll have the interpreter at your
system's disposal. Try the program option `-h` for usage notes.

Run code from a file:

    langnd -f hello.txt

Run code from text:

    langnd -t 'import "write" from core $write("hello world\n", 1)'
