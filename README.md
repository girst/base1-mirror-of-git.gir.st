# base1

Reimplementation of [ferno][1]'s [`base1`][2] in C for UNIXoid shells. 

## Why
 * Why not?
 * guaranteed to be compatible with __any__ character set, no matter how small
   (theoretically)

## Building
To generate the binary, run `make`. Then, copy the resulting `base1` binary to
a directory in your `$PATH`.

## Usage
`base1` uses GNU's [`base64`][4] tool as a reference and therefore has similar
command line arguments. This means, you can either pipe data in or give a file
as an argument. 

The non-standard parameter `-t` will return the length of the string to be
encoded, or warn if it will overflow. 

### 'Hello World'-Example:
Can't do: even on 64 bit machines the length of the resulting string will be
larger than `unsigned long long int`. I en- and decoded the string `Hallo` - it
took 52 **minutes**!

    echo -n "Hi" | ./base1 -w 0

will output 18794 `A`s.

## License
`base1` is released under the GNU General Public License, version 3.    
See `LICENSE` for details.
&copy; 2016 Tobias Girstmair, [isticktoit.net][5]

## In other Languages
 * [JavaScript](https://github.com/ferno/base1)

[1]: https://github.com/ferno/
[2]: https://github.com/ferno/base1
[3]: https://twitter.com/girstmair
[4]: http://linux.die.net/man/1/base64
[5]: http://isticktoit.net/
