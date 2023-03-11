# langnd

> langnd is one programmer's dream programming language that focuses on
> minimalism, consistency, and portability.

This project is the home of the langnd programming language. Both the design
and the implementation of the language are a work in progress.

    #!/usr/local/bin/langnd

    $warn=
    <
      # writes a message with a highlighted warning
      $message=argument
      import "write" from core
      $write("\a027[7;33m WARN \a027[0m ", 1)
      $write($message, 1)
      $write("\n", 1)
    >

    $warn("writing langnd code is addictive!")

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
